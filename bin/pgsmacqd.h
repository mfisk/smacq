/********************************************************
 * smacqd.h						*
 *							*
 * This is for my little part of the smacq		*
 * 							*
 ********************************************************/

#define MAXPENDING 5
#define RCVBUFSIZE 32
/* This is a wierd number that Postgres used */
#define BUFFER_SIZE 8192
/* 
 * This query size thing needs to go and I need to make
 * the strech string thing work
 */
#define QUERYSIZE 64
#define INITQUERYSIZE 1024
#define FIELD_SIZE 64 

#define PROTOCOL(m,n) (((m) << 16)|(n))
#define NEGOTIATE_SSL PROTOCOL(1234,5679)

#define PG_PORT 5432 /* TCP port number for PostgreSQL */

#ifndef bool
typedef char bool;
#endif

#ifndef true
#define true ((bool) 1)
#endif

#ifndef false
#define false ((bool) 0)
#endif

typedef struct fields {
    char * name;
    int length;
    struct fields * next;
} fieldRow;

static unsigned char recvBuffer[BUFFER_SIZE];
static unsigned char sendBuffer[BUFFER_SIZE];
static int recvPointer;
static int sendPointer;
static int recvLength;

void startup(int clientSocket, bool SSLdone);
void initQuery(int clientSocket);
void queryLoop(int clientSocket);
int getbytes(int clientSocket, char *s, size_t len);
char getbyte(int clientSocket);
int getstring(int clientSocket, char *s, int len);
static int recvbuf(int clientSocket);
int flush(int clientSocket);
int putbytes(int clientSocket, const char *s, size_t len);
void appendString(char *s, int len);
void dummyParse();
void strechString(char * s, int neededLen);
void completedResponse(int clientSocket, char *message);
void readyForQuery(int clientSocket);
void cursorResponse(int clientSocket,char *message);
void rowDescription(int clientSocket,short numFields, char *fieldNames[]);
void rowDescriptionLL(int clientSocket,short numFields, fieldRow *fieldNames);
void asciiRow(int clientSocket,short numFields, char *fields[]);
void queryHandle(int clientSocket);
void interruptSignalHandler(int signalType);
void handleQuery(char * query);
int findWord(char * buf,char * word);
void asciiRowLL(int clientSocket,short numFields, fieldRow *fieldName);
void add(fieldRow **f, char *c);
void deleteList(fieldRow **);
void add_ith(fieldRow **f,int index, char *c);
