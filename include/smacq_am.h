#include <smacq_gasnet.h>

#define AM_DTSOBJECT 200
#define AM_ENDINPUT 201
#define AM_GETFIELD 202
#define AM_GETTYPE 203
#define AM_INT_REPLY 204
#define AM_QUERY 205
#define AM_DTSOBJECT_TOCHILDREN 205

extern GASNet Gasnet;
//extern DTS AM_DTS;
//extern SmacqScheduler AM_Sched;
//extern bool AM_Idle;

void init_am(int * argc, char *** argv);
void smacq_am_slave_loop();

