#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <zlib.h>
#include <smacq.h>
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

class socketModule : public SmacqModule {
 public:
  socketModule(struct smacq_init * context);
  ~socketModule();

  smacq_result consume(DtsObject *, int *);
  smacq_result produce(DtsObject **, int *);

 private:
  
  int close_it(int closefd);
  void server_init(int port);
  void client_init(int port, char * hostname);

  DtsObject * datum;
  struct pickle *pickle; 
  int *client_type_array;
  int client_array_size;
  fd_set rfds;
  int max_fd;
  int listen_fd, connect_fd;
  int serverd;
  int do_produce;                  /* Does this instance produce */

};


static struct smacq_options options[] = {
  {"p", {int_t:3000}, "Port Number", SMACQ_OPT_TYPE_INT},
  {"h", {string_t:"0.0.0.0"}, "Host Name", SMACQ_OPT_TYPE_STRING},
  {"d", {boolean_t:0}, "Server Daemon", SMACQ_OPT_TYPE_BOOLEAN},
  {NULL, {string_t:NULL}, NULL, 0}
};

int socketModule::close_it(int closefd) { 
  int i;
  close(closefd);
  FD_CLR(closefd, &(rfds));
  pickle_close_source(pickle, closefd);
  if (serverd) {
    if (closefd == max_fd) {
      for (i = (closefd - 1); i >= 0; i--)
	if (FD_ISSET(i, &(rfds))) {
	  max_fd = i; 
	  return 1;
	}
      fprintf(stderr,"Error finding new max, exiting\n");
      exit(-1); /* no new max found */
    }
  }
  return 1;
}

smacq_result socketModule::produce(DtsObject ** datump, int * outchan) {
  struct sockaddr_in their_addr;
  int i, new_fd, temp;
  int sin_size;
  int num_ready_fds, picked_fd = 0;
  fd_set tempset;  

  sin_size = sizeof(struct sockaddr_in);
  
  while (1) {
    tempset = rfds;	
    if ((num_ready_fds = select((max_fd + 1), &tempset, NULL, NULL, NULL)) < 0) {
      perror("select");
      fprintf(stderr,"Error: select\n");
      exit(-1);
    }
    assert(num_ready_fds);
    /* find the lowest fd ready */
    for (i = 0; i <= max_fd; i++) {
      if (FD_ISSET(i, &tempset)) {
	picked_fd = i;
	break; /* found one */
      }
    }
    if (picked_fd == listen_fd) {             /* new client */
      if ((new_fd = accept(listen_fd, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
	fprintf(stderr, "Error: server accept\n");
	exit(-1);
      } 
      FD_SET(new_fd, &(rfds));	
      
      if (new_fd > max_fd) {
	max_fd = new_fd;
      }					
      
      if (!serverd) {      /* if non-daemon server */
	FD_CLR(listen_fd, &(rfds));
	close(listen_fd);
      }
      continue; /* go back to select */
    }
    
    *datump = dts->readObject(pickle, picked_fd);
    if (temp == 0) {
      close_it(picked_fd);  
      
      if (serverd) continue; 
      else return(SMACQ_END);
    } else if (temp < 0) {
      close_it(picked_fd);  

      fprintf(stderr,"Error receiving header from client, dropping connection\n");
      if (serverd) continue; 
      else return(SMACQ_ERROR);
    }
    
    return(SMACQ_PASS);
  } /* while */
}

smacq_result socketModule::consume(DtsObject * datum, int * outchan) {
  assert(datum->write(pickle, connect_fd) > 0);

  return SMACQ_FREE;
}

socketModule::~socketModule() {
  int i;

  for (i = 0; i <= max_fd; i++) {
	if (FD_ISSET(i, &(rfds))) close(i);
  }
}

void socketModule::server_init(int port) {
  struct sockaddr_in my_addr;
  
  do_produce = 1;

  if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Error: server socket\n");
        exit(-1);
  }
  my_addr.sin_family = AF_INET;
  my_addr.sin_port   = htons(port);
  my_addr.sin_addr.s_addr = INADDR_ANY;
  memset(&(my_addr.sin_zero), '\0', 8);

  if (bind(listen_fd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
        fprintf(stderr, "Error: server bind\n");
        exit(-1);
  }

  if (listen(listen_fd, BACKLOG) == -1) {
        fprintf(stderr, "Error: server listen\n");
        exit(-1);
  }

  FD_ZERO(&(rfds));
  FD_SET(listen_fd, &(rfds));
  max_fd = listen_fd;
}

void socketModule::client_init(int port, char * hostname) {
  int host;
  struct sockaddr_in their_addr;
  struct hostent *hostn;

  //fprintf(stderr, "Initiating Client\n");
  if ((connect_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
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

  if (connect(connect_fd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
        fprintf(stderr, "Error: client connect\n");
        exit(-1);
  }

  client_array_size = 1;
  client_type_array = (int*)malloc(sizeof(int));
  *client_type_array = 0; 
}

socketModule::socketModule(struct smacq_init * context) : SmacqModule(context) {
  smacq_opt port, hostname, serverd_opt;
  
  {
    struct smacq_optval optvals[] = {
      { "p", &port},
      { "h", &hostname},
      { "d", &serverd_opt},
      {NULL, NULL}
    };
    
    //XXX: for some reason the following nulls state 	
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 NULL, NULL,
				 options, optvals);
    
  }
  
  serverd = serverd_opt.int_t;

  pickle = pickle_init();
  
  if (context->isfirst) {  
    if (context->islast) 
      fprintf(stderr, "Error: Empty pipeline, no socket required\n");
    server_init(port.int_t);
  } else if (context->islast)
    client_init(port.int_t, hostname.string_t);
  else {
    fprintf(stderr, "Error: Middle of Local Pipeline, no socket required\n");
    exit(-1);
  } 
}

static SmacqModule * socket_constructor(struct smacq_init * context) {
  return new socketModule(context);
}

struct smacq_functions smacq_socket_table = {
  constructor: &socket_constructor,
};

