#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <smacq.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netdb.h>

#define MAX_LINE 1e7 /* == 1e7 == 10MB */

struct get_line {
	FILE * fh;
	int buffer_size;
	int buffer_used;
	int leading;
	char * read_buffer;
};

SMACQ_MODULE(disarm,
  PROTO_CTOR(disarm);
  PROTO_PRODUCE();

  smacq_graph * self;
  int datasock;
  FILE * datafh;
  unsigned long lineno;
  int sv4_type;
  struct get_line linebuf;
  char * date_string;
  char * srcip_string;
  char * dstip_string;
  char * srcport_string;
  char * dstport_string;

  void init_get_line(struct get_line * s, FILE * fh);
  int get_line(char ** buf, struct get_line * s);
  int server_init(struct sockaddr_in * addrp);
  int client_init(int port, char * hostname, char ** ipstr);
  static void filter_callback(char * op, int argc, char ** argv, void * data);
);

static struct smacq_options options[] = {
  {"f", {string_t:""}, "Input File", SMACQ_OPT_TYPE_STRING},
  {"date", {string_t:""}, "Date", SMACQ_OPT_TYPE_STRING},
  {"dstport", {string_t:""}, "Destination Port", SMACQ_OPT_TYPE_STRING},
  {"srcport", {string_t:""}, "Source Port", SMACQ_OPT_TYPE_STRING},
  {"dstip", {string_t:""}, "Destination IP", SMACQ_OPT_TYPE_STRING},
  {"srcip", {string_t:""}, "Source IP", SMACQ_OPT_TYPE_STRING},
  {"p", {int_t:9096}, "Port Number", SMACQ_OPT_TYPE_INT},
  {"h", {string_t:"disarm5.lanl.gov"}, "Host Name", SMACQ_OPT_TYPE_STRING},
  END_SMACQ_OPTIONS
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

#define GETLINEBUFSIZE MAX_LINE

void disarmModule::init_get_line(struct get_line * s, FILE * fh) {
	s->fh = fh;
	s->buffer_size = GETLINEBUFSIZE;
	s->read_buffer = malloc(s->buffer_size);
	s->leading = 0;
	s->buffer_used = 0;
}

/* Fills buffer upto a newline or EOF.  There is no NULL terminator. */
/* Return value is string length */
int disarmModule::get_line(char ** buf, struct get_line * s) {
	/* 
	 * What we need is an fgets that stops at newlines or NULLs 
	 * since NULLs in a line screws up fgets. 
	 * Having to use fgetc is really slow, so we to do our own buffering.
         */
	int already_checked = s->leading;
	*buf = NULL; /* For safety */

	while (1) {
	    char * offset = (char*)memchr(s->read_buffer + already_checked, '\n', s->buffer_used - already_checked);
	    if (offset) {
		int len = offset - (s->read_buffer + s->leading) + 1;
		*buf = s->read_buffer + s->leading;
		s->leading += len;
		return len;
	    } else {
		int got;
		if (s->leading) {
			s->buffer_used -= s->leading;
			memmove(s->read_buffer, s->read_buffer + s->leading, s->buffer_used);
			s->leading = 0;
		}
		if (s->buffer_used == s->buffer_size) {
			/* Not a whole line, but buffer is full */
			*buf = s->read_buffer;
			s->leading = 0;
			s->buffer_used = 0;
			fprintf(stderr, "disarm: error: line too long (> %d bytes)\n", s->buffer_size);
			return 0;
		}
		got = fread(s->read_buffer + s->buffer_used, 1, s->buffer_size - s->buffer_used, s->fh);
		if (got > 0) {
			already_checked = s->buffer_used;
			s->buffer_used += got;
			/* Iterate and check for null */
		} else {
			/* EOF or something terminal */
			*buf = s->read_buffer;
			s->buffer_used = 0;
			return s->buffer_used;
		}
	    }
	}
}

smacq_result disarmModule::produce(DtsObject & datump, int * outchan) {
	char * hex;
	unsigned char * decode;
	int i;
	int len;
	DtsObject datum;

	len = get_line(&hex, &linebuf);
	if (len == 0) {
		return SMACQ_END;
	}

	lineno++;

	if (len < 98) {
		fprintf(stderr, "Skipping invalid %d char sv4 record on line %lu: %.*s\n", len, lineno, len, hex);
		return produce(datump, outchan);
        }

	//fprintf(stderr, "Got sv4 hex line: %s\n", hex);

	/* Now we decode the hex into the binary data object */

	len -= 49;
  	datum = dts->newObject(sv4_type, len);
	decode = datum->getdata();

	for (i=0; i<49; i++) {
	 	unsigned char c = hex2val[(unsigned int)hex[i*2]];	
		//fprintf(stderr, "char %c has value %d\n", hex[i*2], c);
		if (c == XX) {
			fprintf(stderr, "Skipping %d char sv4 record on line %lu; character %d has invalid code 0x%hhx: %.*s\n", len, lineno, i*2, hex[i*2], len, hex);
			//fprintf(stderr, "line %ld character %d invalid: %c\n", lineno, i*2, hex[i*2]);
			return produce(datump, outchan);
		}
		decode[i] = c << 4;
		c = hex2val[(unsigned int)hex[i*2+1]];
		//fprintf(stderr, "char %c has value %d\n", hex[i*2+1], c);
		assert(c != XX);
		decode[i] |= c;
	}

	memcpy(decode+49, hex+98, len-49);

	datump = datum;
	return (smacq_result)(SMACQ_PASS|SMACQ_PRODUCE);
}

int disarmModule::server_init(struct sockaddr_in * addrp) {
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
  if (-1 == getsockname(listen_fd, (struct sockaddr*)addrp, &len)) {
        perror("disarm server getsockname\n");
        exit(-1);
  }

  return listen_fd;
}

int disarmModule::client_init(int port, char * hostname, char ** ipstr) {
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
  if (-1 == getsockname(client_fd, (sockaddr*)&myaddr, &len)) {
        perror("disarm client getsockname");
        exit(-1);
  }
	
  *ipstr = (char *)inet_ntop(AF_INET, &myaddr.sin_addr, *ipstr, INET_ADDRSTRLEN);
  assert(*ipstr);

  return(client_fd);
}

void disarmModule::filter_callback(char * op, int argc, char ** argv, void * data) {
  disarmModule * ths = (disarmModule*)data;
  if (!strcmp(op, "equals") && argc == 3) {
	  if (!strcmp(argv[1], "srcip")) {
		  ths->srcip_string = strdup(argv[2]);
		  //fprintf(stderr, "Desired srcip is %s\n", argv[2]);
 	  } else if (!strcmp(argv[1], "date")) {
		  ths->date_string = strdup(argv[2]);
	          //fprintf(stderr, "Desired date is %s\n", argv[2]);
  	  }
  } else {
  	fprintf(stderr, "Unknown filter callback: %s + %d args\n", op, argc);
  }
}

disarmModule::disarmModule(struct smacq_init * context) : SmacqModule(context) {
  smacq_opt infile, port, hostname;
  char * end_date;

  self = context->self;
  {
    smacq_opt date, srcip, dstip, dstport, srcport;
    struct smacq_optval optvals[] = {
      { "f", &infile},
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

  	if (strcmp("", date.string_t)) {
		date_string = date.string_t;
  	}
  	if (strcmp("", srcip.string_t)) {
		srcip_string = srcip.string_t;
  	}
  	if (strcmp("", dstip.string_t)) {
		dstip_string = dstip.string_t;
  	}
  	if (strcmp("", srcport.string_t)) {
		srcport_string = srcport.string_t;
  	}
  	if (strcmp("", dstport.string_t)) {
		dstport_string = dstport.string_t;
  	}
  }

  /* Get downstream filters before we apply args */
  smacq_downstream_filters(self, filter_callback, this);

  sv4_type = dts->requiretype("sv4");

  if (!strcmp(infile.string_t, "-")) {
	datafh = stdin;
  } else if (strcmp(infile.string_t, "")) {
	datafh = fopen(infile.string_t, "r");
        assert(datafh);
  } else {
  	struct sockaddr_in myaddr, server_addr;
	int listen_fd, fd;
	FILE * fh;
	char myip_buf[INET_ADDRSTRLEN];
	char * myip = myip_buf;
	int sin_size;

  	if (!date_string) {
		fprintf(stderr, "disarm: date must be specified!\n");
		exit(-1);
  	}

  	end_date = index(date_string, '~');
  	if (end_date) {
			end_date[0] = '\0';
			end_date++;
  	}

  	listen_fd = server_init(&myaddr);
	fd = client_init(port.int_t, hostname.string_t, &myip);
	fh = fdopen(fd, "w");
	fprintf(fh, "<Query");

	if (srcport_string)
		fprintf(fh, " srcport=\"%s\"", srcport_string);
	if (dstport_string)
		fprintf(fh, " dstport=\"%s\"", dstport_string);
	if (srcip_string)
		fprintf(fh, " srcip=\"%s\"", srcip_string);
	if (dstip_string)
		fprintf(fh, " dstip=\"%s\"", dstip_string);

	fprintf(fh, "><Date start=\"%s\"", date_string);
	if (end_date && strcmp("", end_date)) { 
		fprintf(fh, " end=\"%s\"", end_date);
	}
	
	fprintf(fh, "/> <SocketResult type=\"tcp\" host=\"%s\" port=\"%d\" />", myip, ntohs(myaddr.sin_port));
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

    	if ((datasock = accept(listen_fd, (struct sockaddr *)&server_addr, &sin_size)) == -1) {
		perror("disarm server accept");
		exit(-1);
    	} 
	datafh = fdopen(datasock, "r");
  }

  init_get_line(&linebuf, datafh);
}

