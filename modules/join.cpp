#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <map>

#include <SmacqModule.h>
#include <FieldVec.h>
#include <SmacqGraph.h>
#include <SmacqScheduler.h>

struct alias {
  DtsField field;
  DtsField newfield;
  SmacqGraph * until;
  std::vector<DtsObject> objects;
};

SMACQ_MODULE(join,
	     PROTO_CTOR(join);
	     PROTO_CONSUME();

	     private:

	     SmacqGraph * where;
	     dts_typeid emptytype;

	     std::vector<struct alias> Aliases;

	     void for_all_but(unsigned int is_alias, DtsObject o, unsigned int alias);
	     ); 

// Cross-product.  This is about the least efficient way to implement this, but
// it works for a first version.  We construct all the possible cross-products
// of the aliases and then test each combination against the entire where clause.
//
// XXX:  We need only evaluate expressions that we haven't already evaluated for
// the new object.
//
void joinModule::for_all_but(unsigned int is_alias, DtsObject o, unsigned int alias) {
  if (alias == Aliases.size()) {
    // Base case; test this join.
    if (!where || (SMACQ_PASS == scheduler->decideContainer(where, o))) {
      // Dup the object, because we're about to assign new alias values.
      DtsObject cpy = o->dup();
      //fprintf(stderr, "passing joined obj %p\n", cpy.get());
      enqueue(cpy, 0);	
    }
    return;
  }

  if (is_alias != alias) {
    for (unsigned int j = 0; j < Aliases[alias].objects.size(); j++) {
      o->attach_field(Aliases[alias].field, Aliases[alias].objects[j]);
      assert(Aliases[alias].objects[j]);
      //fprintf(stderr, "attached %p to %p\n", Aliases[alias].objects[j].get(), o.get());
      for_all_but(is_alias, o, alias+1);
    }
  } else {
    for_all_but(is_alias, o, alias+1);
  }
}
  
smacq_result joinModule::consume(DtsObject datum, int & outchan) {
  // Find which alias we just got a new input for
  unsigned int num_aliases = Aliases.size();
  for (unsigned int i=0; i < num_aliases; i++) {
    alias & a = Aliases[i];

    DtsObject o = datum->getfield(a.field, true);
    if (!o) continue;

    // Remove any stored objects that pass UNTIL graph.
    // XXX.  This only tests against new instances of same alias.
    if (a.until) {
      // Make a new test object
      DtsObject t = dts->newObject(emptytype);
      // Attach this object as the "new" object
      t->attach_field(a.newfield, datum);

      // Test each existing object
      for (unsigned int j=0; j < a.objects.size(); j++) {
        // Build test join
        t->attach_field(a.field, a.objects[j]);

        if (SMACQ_PASS == scheduler->decideContainer(a.until, t)) {
	  // Remove j'th element
	  // (by swapping and shrinking)
	  a.objects[j] = a.objects.back();
	  j--;
	  a.objects.pop_back();
        }
      }
    }

    // XXX. Then need to remove objects that used to pass relational 
    // filters, but don't now that another object has expired.
    ;

    // Test pending joins with this object
	// Dup before this because the passed argument will get all aliases attached to it.
	// That would be okay for datum, since we always SMACQ_FREE it, except that
	// future iterations of this for loop would then register false aliases.
    DtsObject join = datum->dup();
    for_all_but(i, join, 0);
		
    // Save this object.
    a.objects.push_back(o);

    //fprintf(stderr, "call %d: obj %p had alias #%d of %d\n", call++, datum.get(), i, num_aliases);
  }

  return SMACQ_FREE;
}

joinModule::joinModule(SmacqModule::smacq_init * context) 
  : SmacqModule(context)
{
  emptytype = dts->requiretype("empty");

  // Size per-alias data-structures
  Aliases.resize((context->argc-2)/2);
  where = (SmacqGraph*)strtoul(context->argv[context->argc-1], NULL, 0);
  if (where) {
    where->init(dts, scheduler);
    //where->print(stderr, 4);
  }

  // Copy list of alias names to list of DtsFields
  for (int i = 1; i < context->argc-1; i+=2) {
    alias & a = Aliases[(i-1)/2];
    a.field = dts->requirefield(context->argv[i]);
    a.newfield = dts->requirefield("new");

    a.until = (SmacqGraph*)strtoul(context->argv[i+1], NULL, 0);
    if (a.until) { // can be NULL
      a.until->init(dts, scheduler);	
    }
  }


}


