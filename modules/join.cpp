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
  SmacqScheduler * until;
  std::vector<DtsObject> objects;
};

SMACQ_MODULE(join,
  PROTO_CTOR(join);
  PROTO_CONSUME();

 private:

  SmacqScheduler * sched;
  SmacqScheduler * where;
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
		// Okay, test this join.
		if (!where || (SMACQ_PASS == where->decide(o))) {
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

		DtsObject o = datum->getfield(a.field);
		if (!o) continue;

		// Remove any stored objects that pass UNTIL graph.
		// XXX.  This only tests against new instances of same alias.
		for (unsigned int j=0; j < a.objects.size(); j++) {
			// Build test join
			DtsObject t = dts->newObject(emptytype);
			t->attach_field(a.newfield, datum);
			t->attach_field(a.field, a.objects[j]);

			if (a.until && (SMACQ_PASS == a.until->decide(t))) {
				// Remove j'th element
				// (by swapping and shrinking)
				a.objects[j] = a.objects.back();
				a.objects.pop_back();
			}
		}

		// XXX. Then need to remove objects that used to pass relational 
		// filters, but don't now that another object has expired.
		;

  		// Test pending joins with this object
		for_all_but(i, datum, 0);
		
		// Save this object.
		// XXX.  Should save only if it passes the appropriate non-relational filters.
		a.objects.push_back(o);

		//fprintf(stderr, "got alias #%d of %d\n", i, Aliases.size());

		break;
  }

  return SMACQ_FREE;
}

joinModule::get_where(SmacqGraph * where_graph, DtsField field) {
 SmacqGraph * more;
 if (children.size() == 1 && children[0].size() == 1) {
    more = children[0][0]->downstream_filter_one(callback, data);
 } else {
    more = NULL;
 }

 if (where_graph->algebra.stateless && where_graph->usesOtherFields(field)) {
    return more;
 } else {
    SmacqGraph * result = SmacqGraph::new_child(where_graph->argc, where_graph->argv);
    if (more) result->add_child(more);
    return result;
 }
}

joinModule::joinModule(SmacqModule::smacq_init * context) 
  : SmacqModule(context), 
    sched(context->scheduler)
{
  emptytype = dts->requiretype("empty");

  // Size per-alias data-structures
  Aliases.resize((context->argc-2)/2);
  SmacqGraph * where_graph = (SmacqGraph*)strtol(context->argv[context->argc-1], NULL, 0);
  if (!where_graph) {
		where = NULL;
  } else {
		where = new SmacqScheduler(dts, where_graph, false);
		where_graph->init(dts, where);
  		//where_graph->print(stderr, 4);
  }

  // Copy list of alias names to list of DtsFields
  for (int i = 1; i < context->argc-1; i+=2) {
		alias & a = Aliases[(i-1)/2];
		a.field = dts->requirefield(context->argv[i]);
		a.newfield = dts->requirefield("new");
		a.where = get_where(where_graph, a.field);
		SmacqGraph * until_graph = (SmacqGraph*)strtol(context->argv[i+1], NULL, 0);
		if (until_graph) { // can be NULL
			a.until = new SmacqScheduler(dts, until_graph, false);
			until_graph->init(dts, a.until);	
		} else {
			a.until = NULL;
		}
  }


}


