#include <smacq.h>
#include <stdio.h>

int mono_propagate(smacq_graph * f, const dts_object * d) {
  int status;
  int i;
  int retval;
  int outchan = -1;

  if (!f) return 0;
  
  if (d) {
    retval = f->ops.consume(f->state, d, &outchan);
  } else {
    // Force last call
    retval = (SMACQ_PRODUCE|SMACQ_PASS|SMACQ_END);
    //fprintf(stderr, "Doing last call on %s\n", f->name);
  }

  // Suck out all the product we can and take care of it
  if (retval & (SMACQ_PRODUCE|SMACQ_CANPRODUCE)) {
    do {
      int numleft = 0;
      const dts_object * newd;
      status = f->ops.produce(f->state, &newd, &outchan);

      if (! (status & SMACQ_PASS)) { 
	//fprintf(stderr, "%s couldn't produce\n", f->name);
	break;
      }
      
      //fprintf(stderr, "%s produced for child %d\n", f->name, outchan);

      if (outchan >= 0) {
	      assert(outchan < f->numchildren);
	      numleft += mono_propagate(f->child[outchan], newd);
      } else {
      	   for (i=0; i < f->numchildren; i++) 
		numleft += mono_propagate(f->child[i], newd);
      }

      if (!numleft && f->numchildren) { // No more children
	// Tell everybody to stop
	retval |= SMACQ_END;
	status = SMACQ_END;
      }

      dts_decref(newd);
    } while (status & (SMACQ_PRODUCE|SMACQ_CANPRODUCE));
  }
  
  if (retval & SMACQ_PASS) {
    int numleft = 0;
    if (outchan >= 0) {
	      assert(outchan < f->numchildren);
	      numleft += mono_propagate(f->child[outchan], d);
    } else {
   	 for (i=0; i < f->numchildren; i++) 
      		numleft += mono_propagate(f->child[i], d);
    }
    
    if (!numleft && f->numchildren) 
      retval |= SMACQ_END;
  }

  if (retval & SMACQ_END)  {
    if (f->ops.shutdown)
      f->ops.shutdown(f->state);

    return 0;
  }

  return 1;
}


void sched_mono(smacq_graph * objs) {
  mono_propagate(objs, NULL);
}
