/* your preprocessor header file goes here */

#include "spp_export_pkt.h"

void list_enqueue(list_item *);

ExportPktData * exptr;

/*
 * Function: SetupExportPkt()
 *
 * Purpose: Registers the preprocessor keyword and initialization 
 *          function into the preprocessor list.  This is the function that
 *          gets called from InitProprocessors() in plugbase.c.
 *
 * Arguments: None.
 *
 * Returns: void function
 *
 */
void SetupExportPkt()
{
    /*link the preprocessor keyword to the init function in the preproc list*/

    RegisterPreprocessor("export_pkt", ExportPktInit);
    DebugMessage(DEBUG_FLOW, "Preprocessor: ExportPkt is setup...\n");
} 

/* 
 * Function: ExportPktInit(u_char *) 
 * 
 * Purpose: Calls the argument parsing function, performs final setup on data
 *          structs, links the preproc function into the function list.
 *
 * Arguments: state_pkt => ptr to a pkt (eg. in a 'flow' state struct
 *
 * Returns: void function
 *
 */
void ExportPktInit(u_char *state_pkt)
{
    ExportPktData * data;
    
    DebugMessage(DEBUG_FLOW, "Initializing export_pkt\n");

    data = malloc(sizeof(ExportPktData));
	exptr = data;
	//exptr->newpkt = (Packet **)state_pkt;
	exptr->new_item = (list_item **)state_pkt;

	if (!exptr) {
	  ErrorMessage("%s\n", "Problem in ExportPktInit(): NULL ExportPktData*");
	  return;
	}
	else {
      DebugMessage(DEBUG_FLOW, "ExportPktInit():  new_item = %p\n", exptr->new_item);
	}

    /* Set the preprocessor function into the function list */
    AddFuncToPreprocList(ExportPkt);
    AddFuncToCleanExitList(ExportPktCleanExit, NULL);
    AddFuncToRestartList(ExportPktRestart, NULL);

#ifdef DEBUG
    printf("ExportPkt Initialized\n");
	fflush(stdout);
#endif

}

/*
 * Function: PreprocFunction(Packet *)
 *
 * Purpose: Perform the preprocessor's intended function.  This can be
 *          simple (statistics collection) or complex (IP defragmentation)
 *          as you like.  Try not to destroy the performance of the whole
 *          system by trying to do too much....
 *
 * Arguments: p => pointer to the current packet data struct 
 *
 * Returns: void function
 */

void ExportPkt(Packet *p)
{
    list_item * li;

    DebugMessage(DEBUG_STREAM, "ExportPkt: entered...\n");

    if (p) {
	  DebugMessage(DEBUG_STREAM,"ExportPkt: p ID = %d\n",ntohs(p->iph->ip_id));

	  if (p->packet_flags & PKT_REBUILT_STREAM) {
        DebugMessage(DEBUG_STREAM, "PKT_REBUILT_STREAM, ID = %d, len = %d\n", 
		  ntohs(p->iph->ip_id), p->pkth->len);

        li = malloc(sizeof(list_item)); 
        li->packet = p;
	    list_enqueue(li);
      }
    }
    else {
      ErrorMessage("%s\n", "problem in ExportPkt(): NULL Packet*");
    }
}

/*
 * Function: ExportPktCleanExit()
 *
 * Purpose: Cleanup at exit time
 *
 * Arguments: signal => signal that caused this event
 *            arg => data ptr to reference this plugin's data
 *
 * Returns: void function
 */
void ExportPktCleanExit(int signal, void *arg)
{
    ExportPktData *data = (ExportPktData *)arg;

#ifdef DEBUG
    printf("ExportPktCleanExit\n");
#endif

    if (data) free(data);
}

/*
 * Function: ExportPktRestart()
 *
 * Purpose: For restarts (SIGHUP usually) clean up structs that need it
 *
 * Arguments: signal => signal that caused this event
 *            arg => data ptr to reference this plugin's data
 *
 * Returns: void function
 */
void ExportPktRestart(int signal, void *arg)
{
    ExportPktData *data = (ExportPktData *)arg;

#ifdef DEBUG
    printf("ExportPktRestart\n");
#endif

    if (data) free(data);
}
