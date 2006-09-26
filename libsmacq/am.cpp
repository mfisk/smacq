#ifdef USE_GASNET
#include <smacq.h>
#include <SmacqScheduler.h>
#include <smacq_am.h>

typedef void (*handler_t)();

// This is a global context
GASNet Gasnet;

// Some callbacks need instances to use
DTS am_dts;
SmacqScheduler am_sched;
SmacqGraph * am_graph = NULL;

// When things fall of end of graph, we have to send them back to somebody's children
SmacqGraphNode * CallingGraph = NULL;

// This is used as a semaphore to determine when a slave has work to do
bool AM_Idle = true;

// Handle a reply with an int and place to put it
void am_int_reply_handler(gasnet_token_t token, gasnet_handlerarg_t arg0, gasnet_handlerarg_t arg1) {
        // Hmmm, so much for security....
        *(int*)arg1 = (int)arg0;
}

void am_field_handler(gasnet_token_t token, void * buf, size_t nbytes, gasnet_handlerarg_t arg0) {
        int retval = am_dts.requirefield((char*)buf);
        // Send value back
        Gasnet.ReplyShort(token, AM_INT_REPLY, retval, 1, arg0);
}

void am_type_handler(gasnet_token_t token, void * buf, size_t nbytes, gasnet_handlerarg_t arg0) {
        int retval = am_dts.requiretype((char*)buf);
        // Send value back
        Gasnet.ReplyShort(token, AM_INT_REPLY, retval, 1, arg0);
}

void am_query_handler(gasnet_token_t token, void * querybuf, size_t nbytes, gasnet_handlerarg_t seed_produce, SmacqGraphNode * node) {
        SmacqGraph * g = SmacqGraphNode::newQuery(&am_dts, &am_sched, 1, (char**)&querybuf);
	CallingGraph = node;

	if (seed_produce) {
		am_sched.seed_produce(g);
		AM_Idle = false;
	}
	am_graph = g;
}

void am_dtsobject_handler(gasnet_token_t token, void * object, size_t nbytes, gasnet_handlerarg_t object_type) {
	DtsObject d = am_dts.newObject(object_type, nbytes);
	memcpy(d->getdata(), object, nbytes);
	assert(am_graph);
	am_sched.input(am_graph, d);
	AM_Idle = false;
}

void am_dtsobject_tochildren_handler(gasnet_token_t token, void * object, size_t nbytes, gasnet_handlerarg_t object_type, gasnet_handlerarg_t graph) {
	DtsObject d = am_dts.newObject(object_type, nbytes);
	memcpy(d->getdata(), object, nbytes);
	SmacqGraphNode * CallingGraph = (SmacqGraphNode*)graph;
	am_sched.queue_children(CallingGraph, d, 0);
	AM_Idle = false;
}

void am_endinput_handler(gasnet_token_t token, void * object, size_t nbytes, gasnet_handlerarg_t object_type) {
	am_graph->shutdown();
	AM_Idle = false;
}

// Kick things off
void init_am(int * argc, char *** argv) {
	am_dts.use_master();

	Gasnet.registerHandler((handler_t)am_query_handler, AM_QUERY);
	Gasnet.registerHandler((handler_t)am_field_handler, AM_GETFIELD);
	Gasnet.registerHandler((handler_t)am_type_handler, AM_GETTYPE);
	Gasnet.registerHandler((handler_t)am_int_reply_handler, AM_INT_REPLY);
	Gasnet.registerHandler((handler_t)am_dtsobject_handler, AM_DTSOBJECT);
	Gasnet.registerHandler((handler_t)am_dtsobject_tochildren_handler, AM_DTSOBJECT_TOCHILDREN);
	Gasnet.registerHandler((handler_t)am_endinput_handler, AM_ENDINPUT);

	//Gasnet.attach(argc, argv);

	fprintf(stderr, "Parallel SMACQ using %d nodes\n", gasnet_nodes());
}

// Slave processes go here
void smacq_am_slave_loop() {
	fprintf(stderr, "Slave node %d ready\n", gasnet_mynode());
	for (;;) {
		DtsObject dout;
		smacq_result r;
		GASNET_BLOCKUNTIL(!AM_Idle);
		for (;;) {
			r = am_sched.element(dout);
			if (r & SMACQ_PASS) {
				Gasnet.RequestMedium(0, AM_DTSOBJECT, dout->getdata(), dout->getsize(), dout->gettype(), CallingGraph);
			} else if (r == SMACQ_NONE) {
				GASNET_BLOCKUNTIL(!AM_Idle);
			}
		}
	}
}

extern "C" {
	gasnet_handlerentry_t * upcri_get_handlertable() {
		return NULL;
	}
	gasnet_handlerentry_t * upcri_get_handlertable_count() {
		return 0;
	}
}
#endif
