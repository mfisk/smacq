/********************************************************
 * smacqd.c						*
 *							*
 * Eric Nelson						*
 * neric@lanl.gov					*
 *							*
 * This is a PgSQL daemon front-end for smacq.          *
 * It is in a beta stage now, but it appears useable.	*
 * 							*
 ********************************************************/
#define SMACQPATH "/home/eric/smacq/build/Linux-ppc/bin/smacqq"
#define DEBUG

#include <smacq.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <malloc.h>

#ifdef DEBUG
#include<mcheck.h>
#endif

#include"pgsmacqd.h"

/*
 * this is a global for the signal handler, and then I got lazy
 * and used it for the handleQuery function.  I think I should
 * convert everything to use this global.
 */
static int sock;

int main(int argc, char *argv[])
{
    int servSock;
    int clntSock;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in echoClntAddr;
    unsigned short echoServPort;
    unsigned int clntLen;
    struct sigaction handler;

#ifdef DEBUG
    mtrace();
#endif
    if (argc == 1) {
	echoServPort = PG_PORT;
    } else if (argc == 2) {
	echoServPort = atoi(argv[1]);
    } else {
	fprintf(stderr,"Usages: %s <Server IP>\n",argv[0]);
	exit(1);
    }
    /* handle signals */
    handler.sa_handler = interruptSignalHandler;
    if(sigfillset(&handler.sa_mask) < 0) {
	fprintf(stderr,"sigfillset() failed\n");
	exit(1);
    }
    /* uhh */
    handler.sa_flags = 0;
    if (sigaction(SIGQUIT,&handler,0) < 0) {
	fprintf(stderr,"sigaction() failed\n");
	exit(1);
    }
    if (sigaction(SIGINT,&handler,0) < 0) {
	fprintf(stderr,"sigaction() failed\n");
	exit(1);
    }
    /*create a socket*/
    if ((servSock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP)) <0)
    {
	fprintf(stderr,"socket() failed\n");
	exit(1);
    }

    /* constucting server address */
    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    echoServAddr.sin_port = htons(echoServPort);

    if (bind(servSock,(struct sockaddr *) &echoServAddr,
		sizeof(echoServAddr)) < 0) {
	fprintf(stderr,"bind() failed\n");
	exit(1);
    }

    if (listen(servSock,MAXPENDING) < 0) {
	fprintf(stderr,"listen() failed\n");
	exit(1);
    }

    fclose(stdin);
    for(;;) {
	clntLen = sizeof(echoClntAddr);

	if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, 
			&clntLen)) < 0) {
	    fprintf(stderr,"accept() failed\n");
	    exit(1);
	}
	/* hack for the interrupt handler.  I could just get rid of
	 * clntSock.  I don't thing it matters too much but it would
	 * be nice to be consistant.  Thing may matter if this gets
	 * threaded. */
	sock = clntSock;

#ifdef DEBUG
	printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
#endif
	startup(clntSock,false);
	initQuery(clntSock);
	queryLoop(clntSock);
    }
    /* not reached */
}

/*
 * startup
 *
 * currently doesn't do much besides tell the frontend that 
 * ssl is not going to be used.
 *
 */
void startup(int clientSocket, bool SSLdone)
{
    struct startuppacket {
	unsigned int protocol;
	char dbname[64];
	char user[32];
	char options[64];
	char unused[64];
	char tty[64];
    } *start;

    int len;
    char c;

    if (getbytes(clientSocket,(char *) &len, 4) == EOF)
	fprintf(stderr,"startup(): bad startup packet\n");

    len = ntohl(len);
    len -= 4;

    if (len > sizeof(struct startuppacket))
	fprintf(stderr,"startup(): bad startup packet length\n");

    if ((start = malloc(sizeof(struct startuppacket))) == NULL) {
	fprintf(stderr,"malloc() failed\n");
	exit(1);
    }

    if (getbytes(clientSocket,(void *)start,len) == EOF)
	fprintf(stderr,"startup(): bad startup packet\n");

    start->protocol = ntohl(start->protocol);
    /* stupid ssl */
    if (start->protocol == NEGOTIATE_SSL && !SSLdone) {
	char no = 'N';
	if ((send(clientSocket, &no, 1, 0)) != 1){
	    fprintf(stderr,"send(): ssl negotiation failed\n");
	    exit(1);
	}

	free(start);
	start = NULL;
	return startup(clientSocket, true);
    }

    /* AuthenticationOk - i should change this */
    c ='R';
    if ((send(clientSocket,&c,sizeof(char),0)) != sizeof(char)){
	fprintf(stderr,"fail R\n");
	exit(1);
    }

    len = 0;
    len = htonl(len);
    if ((send(clientSocket,&len,sizeof(int),0)) != sizeof(int)){
	fprintf(stderr,"fail 0\n");
	exit(1);
    }

    /*backendkeydata - this is a hack.  I hate hacks*/
    /*
    c = 'K';
    if ((send(clientSocket,&c,sizeof(char),0)) != sizeof(char)){
	fprintf(stderr,"fail Z\n");
	exit(1);
    }
    len = getpid();
    len = htonl(len);
    if ((send(clientSocket,&len,sizeof(int),0)) != sizeof(int)){
	fprintf(stderr,"fail 0\n");
	exit(1);
    }
    len = random();
    len = htonl(len);
    if ((send(clientSocket,&len,sizeof(int),0)) != sizeof(int)){
	fprintf(stderr,"fail 0\n");
	exit(1);
    }
    */

    /* ready for query */
    readyForQuery(clientSocket);
    flush(clientSocket);
    free(start);
}

void initQuery(int clientSocket)
{
    char q;
    char *p;
    char *fieldNames[10];

    /* is this something? */
    if ((p = malloc(QUERYSIZE)) == NULL) {
	fprintf(stderr,"malloc() failed\n");
	exit(1);
    }

    getbytes(clientSocket,&q,sizeof(char));
    if (q != 'Q'){
	fprintf(stderr,"initQuery(): This is not a init query!\n");
	exit(1);
    }
    /*
     * uh i'm too lazy to actually parse this, and and i'm just
     * going to assume I know what it is, since smacq sure as
     * hell doesn't.
     *
     * oh damn, I might have to parse this bad boy up myself.
     */
    getstring(clientSocket,p,0);

    //dummyParse(p);
    free(p);
    p = NULL;

    completedResponse(clientSocket,"BEGIN");
    cursorResponse(clientSocket,"blank");

    if ((fieldNames[0] = malloc(20)) == NULL) {
	fprintf(stderr,"malloc() failed\n");
	exit(1);
    }
    strcpy(fieldNames[0],"getdatabaseencoding");
    rowDescription(clientSocket,1,fieldNames);

    strcpy(fieldNames[0],"SQL_ASCII");
    asciiRow(clientSocket,1,fieldNames);

    completedResponse(clientSocket,"SELECT");
    completedResponse(clientSocket,"COMMIT");
    readyForQuery(clientSocket);

    flush(clientSocket);
    free(fieldNames[0]);
}

/*
 * getbytes
 *
 * this gets len number of bytes from the socket and places
 * them into the buffer s.  recvbuf is called and it does
 * actual recv calls
 */
int getbytes(int clientSocket, char *s, size_t len)
{
    size_t amount;

    while (len > 0)
    {
	while (recvPointer >= recvLength)
	{
	    if (recvbuf(clientSocket))
		return EOF;
	}
	amount = recvLength - recvPointer;
	if (amount > len)
	    amount = len;
	memcpy(s, recvBuffer + recvPointer, amount);
	recvPointer += amount;
	s += amount;
	len -= amount;
    }
    return 0;
}

char getbyte(int clientSocket)
{
    while(recvPointer >= recvLength) {
	if (recvbuf(clientSocket))
	    return EOF;
    }
    return recvBuffer[recvPointer++];
}

/*
 *
 * recvbuf
 *
 * this gets bytes from clientSocket and puts them in the 
 * recvBuffer buffer
 *
 */
static int recvbuf(int clientSocket)
{
    if (recvPointer > 0) {
	if (recvLength > recvPointer) {
	    /*left-justify*/
	    memmove(recvBuffer, recvBuffer + recvPointer,
		    recvLength - recvPointer);
	    recvLength -= recvPointer;
	    recvPointer = 0;
	}
	else
	    recvLength = recvPointer = 0;
    }

    for(;;) {
	int n;

	n = recv(clientSocket, recvBuffer + recvLength,
		BUFFER_SIZE - recvLength, 0);
	if(n <= 0){
	    fprintf(stderr,"recvbuf(): failed\n");
	    return EOF;
	}

	recvLength += n;
	return 0;
    }
}

/*
 * This is broken!
 * why do I ask for len?  i'm not using it at all
 */
int getstring(int clientSocket, char *s, int len)
{
    int i;
    char q;
    q = '\0';
    for(;;){
	while (recvPointer >= recvLength) {
	    if (recvbuf(clientSocket))
		return EOF;
	}
	for (i= recvPointer; i < recvLength; i++){
	    if ((recvBuffer[i]) == '\0') {
		/*pqcomm does something here with the
		 * appendBinaryStringInfo
		 * a memcpy to s would probably be a good thing
		 */
		appendString(s,i);
		recvPointer = i+1;
		return 0;
	    }
	}
	/*
	 * we need a memcpy, and we haven't gotten to the \0 yet
	 */
	recvPointer = recvLength;
    }
}

void appendString(char *s, int len)
{
    /*
     * make char s as big as len.  I should free the memory of s,
     * and then allocate len + 1, and then do a memcpy, with the data
     * in recvBuffer.
     */
     strechString(s,len+1);
     strncpy(s,recvBuffer + recvPointer,len);
}

int putbytes(int clientSocket, const char *s, size_t len)
{
    size_t amount;

    while (len>0) {
	if (sendPointer >= BUFFER_SIZE)
	    if (flush(clientSocket))
		return EOF;
	amount = BUFFER_SIZE - sendPointer;
	if (amount > len)
	    amount = len;
	memcpy(sendBuffer + sendPointer, s, amount);
	sendPointer += amount;
	s += amount;
	len -= amount;
    }
    return 0;
}

/*
 * flush()
 *
 * This is needed to force the contents of the sendBuffer
 * to the client.
 */
int flush(int clientSocket)
{
    static int last_errno = 0;
    unsigned char *bufptr = sendBuffer;
    unsigned char *bufend = sendBuffer + sendPointer;

    while (bufptr < bufend) {
	int r;

	r = send(clientSocket,bufptr,bufend - bufptr, 0);

	if (r <= 0) {
	    if (errno == EINTR)
		continue;
	    /*
	     * do something
	     * pqcomm.c pq_flush() does something to let it 
	     * continue if it is just interrupted
	     */
	    if (errno != last_errno) {
		last_errno = errno;
		fprintf(stderr,"send() most likely failed.  probably...\n");
	    }

	    sendPointer = 0;
	    return EOF;
	}
	last_errno = 0;
	bufptr += r;
    }
    sendPointer = 0;
    return 0;
}

void queryLoop(int clientSocket)
{
    char query_type;
    
    for(;;){
	query_type = getbyte(clientSocket);
	switch (query_type) {
	    case EOF:
		printf("Am I done? Yes!\n");
		exit(0);
	    case 'Q':
		/*
		 * i need to hand the query off to smacq, and then
		 * send back the results to the frontend.
		 *
		 * Right now, I should just send back a hardcoded
		 * simple table
		 *
		 */
		queryHandle(clientSocket);
		break;
	    case 'X':
		return;
	    case 'F':
	    default:
		printf("This is useless. %c \n",(char)query_type);
	} 
    }
}

/********************************************************
 * queryHandle()					*
 *							*
 *							*
 ********************************************************/
void queryHandle(int clientSocket)
{
    char *s;

    /*
     * blah
     */

    if ((s = malloc(QUERYSIZE)) == NULL) {
	fprintf(stderr,"malloc() failed\n");
	exit(1);
    }
    /*
     * parse s?
     */
    getstring(clientSocket,s,0);
   
    handleQuery(s);
    free(s);
}

void dummyParse(char *str)
{
    /*
     * blah - move a pointer, maybe?
     * strechString? by how much?
     */
}

void strechString(char *s, int neededLen)
{
    static int len_alloced = QUERYSIZE;
    
    if (neededLen < len_alloced)
	return;
    /*
     * will this leak memory? yes!
     */
    while(len_alloced < neededLen) {
	len_alloced = 2*len_alloced;
    }
    s = realloc(s,(size_t)len_alloced);
}

void rowDescriptionLL(int clientSocket,short numFields, fieldRow *fieldName)
{
    char type;
    short size;
    int i, object_id, modifier;
    fieldRow * fname;

    fname = fieldName;
    type = 'T';
    /*
     * this pisses me off since it was working, and then I changed it
     * and it still works.  Should I just ignore it totally.
     */
    object_id = 0x413;
    size = 0xffff; /* this is wrong XXX */
    modifier = 0; /* XXX I have no idea what is going on... */
    putbytes(clientSocket,&type,sizeof(char));
    /*
     * send the int16 for the number of fields, and then send out each
     * field data.
     */
    numFields = htons(numFields);
    putbytes(clientSocket,(char *)&numFields,sizeof(short));
    numFields = ntohs(numFields);
    /* for each field, send:
     *   string the field name
     *   int32 object ID of the field type
     *   in16 the type size
     *   int32 the type modifier
     */
    for(i = 0; i < numFields; i++)
    {
	putbytes(clientSocket,fname->name,strlen(fname->name)+1);
	fname = fname->next;
	object_id = htonl(object_id);
	putbytes(clientSocket,(char *)&object_id,sizeof(int));
	size = htons(size);
	putbytes(clientSocket,(char *)&size,sizeof(short));
	modifier = htonl(modifier);
	putbytes(clientSocket,(char *)&modifier,sizeof(int));
    }
}

void rowDescription(int clientSocket,short numFields, char *fieldNames[])
{
    char type;
    short size;
    int i, object_id, modifier;

    type = 'T';
    /*
     * this pisses me off since it was working, and then I changed it
     * and it still works.  Should I just ignore it totally.
     */
    object_id = 0x413;
    size = 0xffff; /* this is wrong XXX */
    modifier = 0; /* XXX I have no idea what is going on... */
    putbytes(clientSocket,&type,sizeof(char));
    /*
     * send the int16 for the number of fields, and then send out each
     * field data.
     */
    numFields = htons(numFields);
    putbytes(clientSocket,(char *)&numFields,sizeof(short));
    numFields = ntohs(numFields);
    /* for each field, send:
     *   string the field name
     *   int32 object ID of the field type
     *   in16 the type size
     *   int32 the type modifier
     */
    for(i = 0; i < numFields; i++)
    {
	putbytes(clientSocket,fieldNames[i],strlen(fieldNames[i])+1);
	object_id = htonl(object_id);
	putbytes(clientSocket,(char *)&object_id,sizeof(int));
	size = htons(size);
	putbytes(clientSocket,(char *)&size,sizeof(short));
	modifier = htonl(modifier);
	putbytes(clientSocket,(char *)&modifier,sizeof(int));
    }
}

/*
 * asciiRow()
 *
 * Sends a row description of the table to the client.
 */
void asciiRow(int clientSocket,short numFields, char *fields[])
{
    char type;
    //char *bitmap;
    char bitmap;
    int i,size,byte_num;

    byte_num = 0;

    type = 'D';
    putbytes(clientSocket,&type,sizeof(char));
    /*
     * XXX this is a hack. this is done correrctly in the linked list 
     * version of this function
     */
    bitmap = (char)0xffff;
    putbytes(clientSocket,&bitmap,sizeof(char));

    for(i = 0; i < numFields; i++)
    {
	size = htonl(strlen(fields[i]) + 4);
	putbytes(clientSocket,(char *)&size,sizeof(int));
	putbytes(clientSocket,fields[i],ntohl(size) - 4);
    }
}

/*
 * asciiRowLL
 *
 * The linked list version of asciiRow
 */
void asciiRowLL(int clientSocket,short numFields, fieldRow *fieldName)
{
    char type;
    char *bitmap,*bitmaptemp;
    int i,j,size,bitcounter;
    fieldRow *fname;
    fieldRow *fnameBitmap;

    fnameBitmap = fname = fieldName;
    bitcounter = j = 0;
    type = 'D';
    putbytes(clientSocket,&type,sizeof(char));
    if ((bitmap = malloc(1)) == NULL) {
	fprintf(stderr,"malloc() failed\n");
	exit(1);
    }
    bitmap[0] = (char)0x0000;
    
    for(i = 0; i < numFields; i++)
    {
	/*
         * the first field corresponds to bit 7 (MSB) of the 1st byte,
	 * the 2nd field corresponds to bit 6 of the 1st byte,
	 * the 8th field corresponds to bit 0 (LSB) of the 1st byte,
	 * the 9th field corresponds to bit 7 of the 2nd byte, and so on.   
	 */
	if (bitcounter > 7) {
	    j++;
	    bitcounter = 0;
	    if ((bitmaptemp = malloc(j+1)) == NULL) {
		fprintf(stderr,"malloc() failed\n");
		exit(1);
	    }
	    memset(bitmaptemp,0,j+1);
	    memcpy(bitmaptemp,bitmap,(size_t)(j+1));
	    free(bitmap);
	    bitmap = bitmaptemp;
	}
	    
	if (strcmp(fnameBitmap->name,"")) {
	    bitmap[j] = bitmap[j] | (1 << (7 - bitcounter));
	}
	fnameBitmap = fnameBitmap->next;
	bitcounter++;
    }

    putbytes(clientSocket,bitmap,(size_t)(j+1));
    free(bitmap);

    for(i = 0; i < numFields; i++)
    {
	if (strcmp(fname->name,"")) {
	    size = htonl(strlen(fname->name) + 4);
	    putbytes(clientSocket,(char *)&size,sizeof(int));
	    putbytes(clientSocket,fname->name,ntohl(size) - 4);
	}
	fname = fname->next;
    }
}

void completedResponse(int clientSocket, char *message)
{
    char type;

    type = 'C';
    putbytes(clientSocket,&type,sizeof(char));
    putbytes(clientSocket,message,strlen(message)+1);
}

void readyForQuery(int clientSocket)
{
    char type;

    type = 'Z';
    putbytes(clientSocket,&type,sizeof(char));
}

void cursorResponse(int clientSocket,char *message)
{
    char type;

    type = 'P';
    putbytes(clientSocket,&type,sizeof(char));
    /*
     * name of the cursor.  probably 'blank'
     */
    putbytes(clientSocket,message,strlen(message)+1);
}

void emptyQueryResponse(int clientSocket)
{
#define MESS ""
    char type, *message;

    type = 'I';
    putbytes(clientSocket,&type,sizeof(char));
    if ((message = malloc(strlen(MESS) + 1)) == NULL) {
	fprintf(stderr,"malloc() failed\n");
	exit(1);
    }

    strcpy(message,MESS);
    putbytes(clientSocket,message,strlen(message)+1);
    free(message);
}

void errorResponse(int clientSocket,char *message)
{
    char type;

    type = 'E';
    putbytes(clientSocket,&type,sizeof(char));
    putbytes(clientSocket,message,strlen(message)+1);
}

/*
 * interruptSignalHandler()
 * This cleans up the socket, and shutdown the sever
 */
void interruptSignalHandler(int signalType)
{
    printf("Interrupt recieved! Exiting!\n");
    close(sock);
    exit(0);
}

int findWord(char * buf,char * word) {
    /*
     * I want to grab the first word, but I need to know
     * if I have grabbed a empty field.  That should be either
     * 2 tabs in a row, or a tab and a newline
     */
    int i,j;

    i = j = 0;

    /* delete */
    if (buf[i] == '\n')
	return EOF;

    while (buf[i] != '\n') {
	if ((buf[i] != '\0')||(buf[i] != '\t')||(buf[i]!='\n')) {
	    while ((buf[j] != '\t')&&(buf[j] != '\n')) {
		j++;
	    }
	    //strncpy(word,buf + i,j+1);
	    strncpy(word,buf + i,j);
	    if ((*(buf+i+j)) =='\n') {
		word[j] = '\0';
		return EOF;
	    } else {
		word[j] = '\0';
		return 0;
	    }
	}
	/*
	if (buf[i] == '\t') {
	    word = buf + i + 1;
	    if ((buf[i+1]=='\t')||(buf[i+1]=='\n')) {
		return 2;
	    }
		
	    return 0;
	}
	*/
	i++;
    }
    return -1;
}

void add(fieldRow **f, char *c) 
{
    fieldRow *temp;
    fieldRow *prev;
    
    temp = *f;
    while (temp != NULL) {
	prev = temp;
	temp = temp->next;
    }

    if ((temp = malloc(sizeof(fieldRow))) == NULL) {
	fprintf(stderr,"malloc() failed\n");
	exit(-1);
    }
    if ((temp->name = malloc(strlen(c) + 1)) == NULL) {
	fprintf(stderr,"malloc() failed\n");
	exit(-1);
    }
    
    strcpy(temp->name,c);
    temp->length = strlen(c);
    temp->next = NULL;
    if (*f == NULL) {
	*f = temp;
    } else {
	prev->next = temp;
    }
}

void add_ith(fieldRow **f,int index, char *c) 
{
    int i,temp_length;
    fieldRow *temp;
    fieldRow *prev;
    
    temp = *f;
    if (index != 0) {
	for (i=0; i<index; i++) {
	    prev = temp;
	    temp = temp->next;
	}
    } else {
	prev = temp;
    }
    
    temp_length = strlen(c);
    if (temp_length > prev->length) {
	free(prev->name);
	prev->name = malloc(temp_length);
	prev->length = temp_length;
        strcpy(prev->name,c);
    } else {
        strcpy(prev->name,c);
    }
}

void deleteList(fieldRow **head_pointer)
{
    fieldRow *temp;
    fieldRow *head;
    head = *head_pointer;
    while (head != NULL) {
	free(head->name);
	temp = head->next;
	free(head);
	head = temp;
    }
    //free(head_pointer);
    head_pointer = NULL;
}

/*
 * Right now this runs smacqq and reads from stdout.
 * Instead, we should use code from smacqq itself.
 */

void handleQuery(char * query)
{
    bool initial_row = true;
    size_t len = 0;
    ssize_t read;
    int processID;
    int eof_flag;
    int fd[2];
    int length;
    int i,j,k,m;
    char *buf;
    char word[FIELD_SIZE];
    fieldRow *fr;
    fieldRow *flabel;

    fr = NULL;
    flabel = NULL;

    pipe(fd);
    if ((processID = fork()) == -1) {
	fprintf(stderr,"fork() failed\n");
    /* child */
    } else if (processID == 0) {
	close(fd[0]);
	close(1);
	dup(fd[1]);
	execl(SMACQPATH,"smacqq",query,0);
	/*this should never be reached*/
	fprintf(stderr,"execl() failed\n");
	exit(1);
    /* parent */
    } else {
	close(fd[1]);
	/* 
	 * maybe I shouldn't use stdin
	 */
	stdin = fdopen(fd[0],"r");
	cursorResponse(sock,"blank");

	flabel = NULL;
	fr = NULL;

	while((read = getline(&buf,&len,stdin)) != EOF) {
	    i = 0;
	    eof_flag = findWord(buf,word);
#ifdef DEBUG
	    printf("| %s\t",word);
#endif
	    length = strlen(word)+1;
	    if (initial_row) {
		add(&fr,word);
		if ((*(buf+length-1))!='\n') {
		    while (eof_flag != EOF) {
			i++;
			eof_flag = findWord(buf+length,word);
#ifdef DEBUG
			printf("| %s\t",word);
#endif
			length += strlen(word)+1;
			add(&fr,word);
		    }
		}
	    } else {
		k = m;
		i = 0;
		add_ith(&fr,i,word);
		/*
		 * the tab thing isn't quite fixed,
		 * so it's still working the ugly way
		 */
		while (i < k) {
		    eof_flag = findWord(buf+length,word);
#ifdef DEBUG
		    printf("| %s\t",word);
#endif
		    length += strlen(word)+1;
		    i++;
		    add_ith(&fr,i+1,word);
		}
	    }
	    m = i;
	    if (initial_row) {
		for(j = 0; j <= i; j++) {
		    add(&flabel,"NAME");
		}
		rowDescriptionLL(sock,j,flabel);
		initial_row = false;
	    }
	    asciiRowLL(sock,i+1,fr);
#ifdef DEBUG
	    printf("|\n");
#endif
	}
	if(buf) {
	    free(buf);
	    buf=NULL;
	    buf = 0;
	    len = 0;
	}
	fclose(stdin);
	close(fd[0]);
	deleteList(&fr);
	deleteList(&flabel);
	completedResponse(sock,"SELECT");
	readyForQuery(sock);
	flush(sock);
    }
}

void newhandleQuery(char * query) 
{

  struct runq * runq = NULL;
  smacq_graph * graph;
  const dts_object *record;

  dts_environment * tenv = dts_init();
  graph = smacq_build_query(tenv, 1, &query);
  assert(graph);

  smacq_start(graph, ITERATIVE, tenv);

  while (! (SMACQ_END & smacq_sched_iterative(graph, NULL, &record, &runq, 1))) {
	assert(record);
	/* Here's an object.  Now get all the fields and handle them. See modules/print.c for examples. */
	fprintf(stderr, "Got object %p\n", record);

	dts_decref(record);
  }
}

/*EOF*/
