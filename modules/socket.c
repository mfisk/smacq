#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <zlib.h>
#include "smacq.h"
#include "dts_packet.h"

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <netdb.h>
#include <sys/time.h>

#include <pickle.h>

#define BACKLOG 10

struct state {
  dts_object * datum;
  smacq_environment * env;
  struct pickle *pickle; 
  int *client_type_array;
  int client_array_size;
  fd_set rfds;
  int max_fd;
  int listen_fd, connect_fd;
  int serverd;
  int produce;                  /* Does this instance produce */
};


static struct smacq_options options[] = {
  {"p", {int_t:3000}, "Port Number", SMACQ_OPT_TYPE_INT},
  {"h", {string_t:"0.0.0.0"}, "Host Name", SMACQ_OPT_TYPE_STRING},
  {"d", {boolean_t:0}, "Server Daemon", SMACQ_OPT_TYPE_BOOLEAN},
  {NULL, {string_t:NULL}, NULL, 0}
};

static int close_it(int closefd, struct state * state) { 
  int i;
  close(closefd);
  FD_CLR(closefd, &(state->rfds));
  pickle_close_source(state->pickle, closefd);
  if (state->serverd) {
    if (closefd == state->max_fd) {
      for (i = (closefd - 1); i >= 0; i--)
	if (FD_ISSET(i, &(state->rfds))) {
	  state->max_fd = i; 
	  return 1;
	}
      fprintf(stderr,"Error finding new max, exiting\n");
      exit(-1); /* no new max found */
    }
  }
  return 1;
}

static smacq_result socket_produce(struct state * state, const dts_object ** datump, int * outchan) {
  struct sockaddr_in their_addr;
  int i, new_fd, temp;
  int sin_size;
  int num_ready_fds, picked_fd;
  fd_set tempset;  

  sin_size = sizeof(struct sockaddr_in);
  
  while (1) {
    tempset = state->rfds;	
    if ((num_ready_fds = select((state->max_fd + 1), &tempset, NULL, NULL, NULL)) < 0) {
      perror("select");
      fprintf(stderr,"Error: select\n");
      exit(-1);
    }
    assert(num_ready_fds);
    /* find the lowest fd ready */
    for (i = 0; i <= state->max_fd; i++) {
      if (FD_ISSET(i, &tempset)) {
	picked_fd = i;
	break; /* found one */
      }
    }
    if (picked_fd == state->listen_fd) {             /* new client */
      if ((new_fd = accept(state->listen_fd, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
	fprintf(stderr, "Error: server accept\n");
	exit(-1);
      } 
      FD_SET(new_fd, &(state->rfds));	
      
      if (new_fd > state->max_fd) {
	state->max_fd = new_fd;
      }					
      
      if (!state->serverd) {      /* if non-daemon server */
	FD_CLR(state->listen_fd, &(state->rfds));
	close(state->listen_fd);
      }
      continue; /* go back to select */
    }
    
    temp = read_datum(state->env, state->pickle, picked_fd, datump);
    if (temp == 0) {
      close_it(picked_fd, state);  
      
      if (state->serverd) continue; 
      else return(SMACQ_END);
    } else if (temp < 0) {
      close_it(picked_fd, state);  

      fprintf(stderr,"Error receiving header from client, dropping connection\n");
      if (state->serverd) continue; 
      else return(SMACQ_ERROR);
    }
    
    return(SMACQ_PASS);
  } /* while */
}

static smacq_result socket_consume(struct state * state, const dts_object * datum, int * outchan) {
  assert(write_datum(state->env, state->pickle, state->connect_fd, datum) > 0);

  return SMACQ_FREE;
}

static int socket_shutdown(struct state * state) {
  int i;

  for (i = 0; i <= state->max_fd; i++) {
	if (FD_ISSET(i, &(state->rfds))) close(i);
  }
  
  free(state);

  return 0;
}

static void server_init(struct state * state, int port) {
  struct sockaddr_in my_addr;
  
  state->produce = 1;

  if ((state->listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Error: server socket\n");
        exit(-1);
  }
  my_addr.sin_family = AF_INET;
  my_addr.sin_port   = htons(port);
  my_addr.sin_addr.s_addr = INADDR_ANY;
  memset(&(my_addr.sin_zero), '\0', 8);

  if (bind(state->listen_fd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
        fprintf(stderr, "Error: server bind\n");
        exit(-1);
  }

  if (listen(state->listen_fd, BACKLOG) == -1) {
        fprintf(stderr, "Error: server listen\n");
        exit(-1);
  }

  FD_ZERO(&(state->rfds));
  FD_SET(state->listen_fd, &(state->rfds));
  state->max_fd = state->listen_fd;
}

static void client_init(struct state * state, int port, char * hostname) {
  int host;
  struct sockaddr_in their_addr;
  struct hostent *hostn;

  //fprintf(stderr, "Initiating Client\n");
  if ((state->connect_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Error: client socket\n");
        exit(-1);
  }

  if ((hostn = gethostbyname(hostname)) == NULL) {
        fprintf(stderr, "Error: gethostbyname\n");
        exit(-1);
  }

  if ((host = inet_addr(inet_ntoa(*((struct in_addr *)hostn->h_addr)))) == -1) {
        fprintf(stderr, "Error: inet_addr\n");
        exit(-1);
  }

  their_addr.sin_family = AF_INET;
  their_addr.sin_port   = htons(port);
  their_addr.sin_addr.s_addr = host;
  memset(&(their_addr.sin_zero), '\0', 8);

  if (connect(state->connect_fd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
        fprintf(stderr, "Error: client connect\n");
        exit(-1);
  }

  state->client_array_size = 1;
  state->client_type_array = malloc(sizeof(int));
  *state->client_type_array = 0; 
}

static int socket_init(struct flow_init * context) {
  struct state * state;
  smacq_opt port, hostname, serverd;
  context->state = state = (struct state*) calloc(sizeof(struct state),1);
  assert(state);
  
  state->env = context->env;
  {
    struct smacq_optval optvals[] = {
      { "p", &port},
      { "h", &hostname},
      { "d", &serverd},
      {NULL, NULL}
    };
    
    //XXX: for some reason the following nulls state 	
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 NULL, NULL,
				 options, optvals);
    //fprintf(stderr,"state: %p", state);
    
  }
  
  state->serverd = serverd.int_t;

  state->pickle = pickle_init();
  
  if (context->isfirst) {  
    if (context->islast) 
      fprintf(stderr, "Error: Empty pipeline, no socket required\n");
    server_init(state, port.int_t);
  } else if (context->islast)
    client_init(state, port.int_t, hostname.string_t);
  else {
    fprintf(stderr, "Error: Middle of Local Pipeline, no socket required\n");
    exit(-1);
  } 
  return 0;
}

struct smacq_functions smacq_socket_table = {
  &socket_produce, 
  &socket_consume,
  &socket_init,
  &socket_shutdown
};

