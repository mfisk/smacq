#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <smacq.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netdb.h>

struct state {
  smacq_environment * env;
  int datasock;
  FILE * datafh;
  int sv4_type;
};

static struct smacq_options options[] = {
  {"date", {string_t:""}, "Date", SMACQ_OPT_TYPE_STRING},
  {"dstport", {string_t:""}, "Destination Port", SMACQ_OPT_TYPE_STRING},
  {"srcport", {string_t:""}, "Source Port", SMACQ_OPT_TYPE_STRING},
  {"dstip", {string_t:""}, "Destination IP", SMACQ_OPT_TYPE_STRING},
  {"srcip", {string_t:""}, "Source IP", SMACQ_OPT_TYPE_STRING},
  {"p", {int_t:9096}, "Port Number", SMACQ_OPT_TYPE_INT},
  {"h", {string_t:"disarm5.lanl.gov"}, "Host Name", SMACQ_OPT_TYPE_STRING},
  {NULL, {string_t:NULL}, NULL, 0}
};

#define XX 127

static unsigned char hex2val[256] = {
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, /* 0 - 15 */
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, /* 16 - */
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, /* 32 - */
     0, 1, 2, 3,  4, 5, 6, 7,  8, 9,XX,XX, XX,XX,XX,XX, /* 48 - */
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, /* 64 - */
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, /* 80 - */
    XX,10,11,12, 13,14,15,XX, XX,XX,XX,XX, XX,XX,XX,XX, /* 96 - */
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, /* 112 - */
    XX,XX,10,11, 12,13,14,15, XX,XX,XX,XX, XX,XX,XX,XX, /* 128 - */
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, /* 144 - */
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, /* 160 - */
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, /* 176 - */
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, /* 192 - */
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, /* 208 - */
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, /* 224 - */
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, /* 240 - 255 */
};

char * fgets_sock(char * buf, int len, FILE * fh) {
	char * res = buf;
	int left = len;
	while(1) {
		res = fgets(res, left, fh);
		if (res == NULL) return NULL;
		if (strlen(buf) == (len - 1)) return buf;
		if (res[strlen(res)-1] == '\n') return buf;
		left -= strlen(res);
		//fprintf(stderr, "fgets retrying for newline\n");
	}
}

static smacq_result disarm_produce(struct state * state, const dts_object ** datump, int * outchan) {
	char hex[99999];
	unsigned char * decode;
	int i;
	int len;
	const dts_object * datum;
	char * res;

	res = fgets_sock(hex, 99999, state->datafh);
	if (res == NULL) {
		return SMACQ_END;
	}

	//fprintf(stderr, "Got sv4 hex line: %s\n", hex);

	/* Now we decode the hex into the binary data object */

	len = strlen(hex) - 1;
	len -= 49;

  	datum = (dts_object*)smacq_alloc(state->env, len+2, state->sv4_type);
	decode = dts_getdata(datum);

	/* SMACQ uses a uint64 for the leading ID field even though sv4 is really a uint48 */
	decode[0] = '\0';
	decode[1] = '\0';
	decode += 2;

	for (i=0; i<49; i++) {
	 	unsigned char c = hex2val[(unsigned int)hex[i*2]];	
		//fprintf(stderr, "char %c has value %d\n", hex[i*2], c);
		assert(c != XX);
		decode[i] = c << 4;
		c = hex2val[(unsigned int)hex[i*2+1]];
		//fprintf(stderr, "char %c has value %d\n", hex[i*2+1], c);
		assert(c != XX);
		decode[i] |= c;
	}

	memcpy(decode+49, hex+98, len-49);

	*datump = datum;
	return SMACQ_PASS|SMACQ_PRODUCE;
}

static int server_init(struct state * state, struct sockaddr_in * addrp) {
  struct sockaddr_in my_addr;
  int listen_fd;
  int len;
  
  if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	perror("disarm server socket");
        exit(-1);
  }
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = 0; //htons(port);
  my_addr.sin_addr.s_addr = INADDR_ANY;
  memset(&(my_addr.sin_zero), '\0', 8);

  if (bind(listen_fd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
	perror("disarm server bind");
        exit(-1);
  }

  if (listen(listen_fd, 1) == -1) {
	perror("disarm server listen");
        exit(-1);
  }

  len = sizeof(struct sockaddr);
  if (-1 == getsockname(listen_fd, addrp, &len)) {
        perror("disarm server getsockname\n");
        exit(-1);
  }

  return listen_fd;
}

static int client_init(struct state * state, int port, char * hostname, char ** ipstr) {
  int host;
  int client_fd;
  struct sockaddr_in their_addr;
  struct hostent *hostn;
  struct sockaddr_in myaddr;
  int len;

  //fprintf(stderr, "Initiating Client\n");
  if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("disarm client socket\n");
        exit(-1);
  }

  if ((hostn = gethostbyname(hostname)) == NULL) {
        perror("disarm gethostbyname\n");
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

  if (connect(client_fd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
        perror("disarm client connect");
        exit(-1);
  }

  len = sizeof(struct sockaddr);
  if (-1 == getsockname(client_fd, &myaddr, &len)) {
        perror("disarm client getsockname");
        exit(-1);
  }
	
  *ipstr = (char *)inet_ntop(AF_INET, &myaddr.sin_addr, *ipstr, INET_ADDRSTRLEN);
  assert(*ipstr);

  return(client_fd);
}

static smacq_result disarm_init(struct smacq_init * context) {
  struct state * state;
  smacq_opt date, srcip, dstip, dstport, srcport, port, hostname;
  context->state = state = (struct state*) calloc(sizeof(struct state),1);
  assert(state);
  
  state->env = context->env;
  {
    struct smacq_optval optvals[] = {
      { "date", &date},
      { "srcip", &srcip},
      { "dstip", &dstip},
      { "dstport", &dstport},
      { "srcport", &srcport},
      { "p", &port},
      { "h", &hostname},
      {NULL, NULL}
    };
    
    smacq_getoptsbyname(context->argc-1, context->argv+1,
				 NULL, NULL,
				 options, optvals);
    
  }

  state->sv4_type = smacq_requiretype(state->env, "sv4");
  {
  	struct sockaddr_in myaddr, server_addr;
	int listen_fd, fd;
	FILE * fh;
	char myip_buf[INET_ADDRSTRLEN];
	char * myip = myip_buf;
	int sin_size;

  	listen_fd = server_init(state, &myaddr);
	fd = client_init(state, port.int_t, hostname.string_t, &myip);
	fh = fdopen(fd, "w");
	fprintf(fh, "<Query ");

	if (strcmp("", srcport.string_t)) 
		fprintf(fh, "srcport=\"%s\" ", srcport.string_t);
	if (strcmp("", dstport.string_t)) 
		fprintf(fh, "dstport=\"%s\" ", dstport.string_t);
	if (strcmp("", srcip.string_t)) 
		fprintf(fh, "srcip=\"%s\" ", srcip.string_t);
	if (strcmp("", dstip.string_t)) 
		fprintf(fh, "dstip=\"%s\" ", dstip.string_t);

	if (strcmp("", date.string_t)) { 
		fprintf(fh, "date=\"%s\" ", date.string_t);
	} else {
		fprintf(stderr, "disarm: -date is mandatory!\n");
		exit(-1);
	}

	fprintf(fh, "> <SocketResult type=\"tcp\" host=\"%s\" port=\"%d\" />", myip, ntohs(myaddr.sin_port));
	fprintf(fh, "</Query>\n<--DiSARM: end query-->\n");
	fclose(fh);

	sin_size = sizeof(struct sockaddr_in);
	{
		fd_set fds;
		struct timeval tv = {3,0};
		int res;

		do {
			FD_ZERO(&fds);
			FD_SET(listen_fd, &fds);
			res = select(listen_fd + 1, &fds, NULL, NULL, &tv);
		} while (res == -1);

		if (res == 0) /* timeout */ {
			fprintf(stderr, "disarm: Timeout waiting for server connection\n");
			exit(-1);
		} 

		assert(FD_ISSET(listen_fd, &fds));
	}

    	if ((state->datasock = accept(listen_fd, (struct sockaddr *)&server_addr, &sin_size)) == -1) {
		perror("disarm server accept");
		exit(-1);
    	} 
	state->datafh = fdopen(state->datasock, "r");
  }

  return 0;
}

struct smacq_functions smacq_disarm_table = {
	produce: &disarm_produce, 
	init: &disarm_init
};

