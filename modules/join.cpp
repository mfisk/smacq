#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <smacq.h>
#include <FieldVec.h>
#include <SmacqGraph.h>
#include <SmacqScheduler.h>
#include <vector>
#include <map>

SMACQ_MODULE(join,
  PROTO_CTOR(join);
  PROTO_CONSUME();

  SmacqScheduler * sched;
  SmacqGraph * where;

  std::vector<DtsField> Aliases;
  std::vector<std::vector<DtsObject> > Objects;
  std::vector<SmacqGraph*> Expirations;
  std::map<std::vector<bool>, SmacqGraph* > Filters;

 private:
  void for_all_but(unsigned int is_alias, DtsObject o, unsigned int alias);
); 

// Cross-product.  This is about the least efficient way to implement this, but
// it works for a first version.  We construct all the possible cross-products
// of the aliases and then test each combination against the entire where clause.
//
// XXX:  We need only evaluate expressions that we haven't already evaluated for
// the new object.
//
// XXX:  It's inefficient to make a zillion sched->input() calls before letting
// the scheduler run any of them.
//
void joinModule::for_all_but(unsigned int is_alias, DtsObject o, unsigned int alias) {
  if (alias == Aliases.size()) {
	// Okay, test this join.
	// Dup the object, because we're about to assign new alias values.
	sched->input(where, o->dup());	
	return;
  }

  if (is_alias != alias) {
	for (unsigned int j = 0; j < Objects[alias].size(); j++) {
		o->attach_field(Aliases[alias], Objects[alias][j]);
		for_all_but(is_alias, o, alias+1);
	}
  } else {
	for_all_but(is_alias, o, alias+1);
  }
}
  
smacq_result joinModule::consume(DtsObject datum, int & outchan) {
  // Find which alias we just got a new input for
  for (unsigned int i=0; i<Aliases.size(); i++) {
	DtsObject o = datum->getfield(Aliases[i]);
	if (!o) continue;

  	// XXX. Remove any stored objects that pass Expiration filters.
  	;

  	// XXX. Then need to remove objects that used to pass relational 
  	// filters, but don't now that another object has expired.
  	;

  	// Test pending joins with this object
       	for_all_but(i, datum, 0);
		
	// Save this object.
	// XXX.  Should save only if it passes the appropriate non-relational filters.
	Objects[i].push_back(o);

	break;
  }

  return SMACQ_FREE;
}

joinModule::joinModule(struct SmacqModule::smacq_init * context) 
  : SmacqModule(context), 
    sched(context->scheduler)
{
  // Size per-alias data-structures
  Objects.resize(context->argc-2);
  Aliases.resize(context->argc-2);

  // Copy list of alias names to list of DtsFields
  for (int i = 1; i < context->argc-1; i++) {
	Aliases[i-1] = dts->requirefield(context->argv[i]);
  }

  where = NULL;
  where = (SmacqGraph*)strtol(context->argv[context->argc-1], NULL, 0);
  if (!where) {
	// where is null, so just send everything to our children.
	where = context->self->children_as_heads();
  } else {
	// run where tests first, then pass results to our children
  	where->join(context->self->children_as_heads());
  }
  where->init(dts, context->scheduler);

}


