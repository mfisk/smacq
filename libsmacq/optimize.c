#include <smacq.h>

#define SMACQ_DEBUG

struct list {
  smacq_graph * g;
  smacq_graph * parent;
  struct list * next;
};

static smacq_graph * find_branch(smacq_graph *g) {
  if (g->numchildren == 0)
    return NULL;

  if (g->numchildren == 1) 
    return find_branch(g->child[0]);

  return g;
}

/* Return a list of all the tails in the given graph (list=NULL initially) */
static struct list * list_tails(struct list * list, smacq_graph * g, smacq_graph * parent) {
  /* A tail is the highest point in the tree that only has linear (fanout = 1) children */
  int i;

  smacq_graph * b = find_branch(g);

  if (b) {
    for (i = 0; i < b->numchildren; i++) {
      list = list_tails(list, b->child[i], b);
    }

    return list;
  } else {
    /* base case: append self */
    struct list * newl = malloc(sizeof(struct list));
    newl->g = g;
    newl->next = list;
    newl->parent = parent;

    return newl;
  }
}

static int compare_element_names(smacq_graph * a, smacq_graph * b) {
  return !strcmp(a->argv[0], b->argv[0]);
}
  
static int compare_elements(smacq_graph * a, smacq_graph * b) {
  int i;

  if (a->argc != b->argc) 
    return 0;

  for (i=0; i < a->argc; i++) {
    if (strcmp(a->argv[i], b->argv[i])) 
      return 0;
  }
  
  return 1;
}

static int merge_tail_ends(smacq_graph * a, smacq_graph * b) {
  if (!a->numchildren) {
    return compare_elements(a,b);
  }

  if (merge_tail_ends(a->child[0], b->child[0])) {
    if (compare_elements(a,b)) {
      return 1;
    } else {
      /* This is the top of the commonality */
      smacq_replace_child(b, 0, a->child[0]);

      return 0;
    }
  } else {
    return 0;
  }
}

static void merge_tails(struct list * alist, struct list * blist) {
  smacq_graph * a = alist->g;
  smacq_graph * b = blist->g;
  smacq_graph * p;
  int adepth = 0; 
  int bdepth = 0; 
  int i;
  
  /* Get tails of equal length */
  for (p = a; p->numchildren; p = p->child[0]) {
    adepth++;
  }
  
  for (p = b; p->numchildren; p = p->child[0]) {
    bdepth++;
  }
  
  if (bdepth > adepth) {
    for (i=0; i < (bdepth-adepth); i++) {
      b = b->child[0];
    }
  } else if (adepth > bdepth) {
    for (i=0; i < (adepth-bdepth); i++) {
      a = a->child[0];
    }
  } 

  if (merge_tail_ends(a, b)) {
    assert(blist->parent); /* else this would be a head */
    smacq_replace_child(blist->parent, 0, a);
    
    if (blist->next) {
      /* Remove b tail from list of tails */
      struct list * lp = blist->next;
      *blist = *lp;
      free(lp);
    }
  }
}

static void merge_all_tails(smacq_graph * g) {
  struct list * list = NULL;
  struct list * lpa, * lpb;
  smacq_graph * bp;

  /* Get all the tails we have */
  for (bp = g; bp; bp=bp->next_graph) {
    list = list_tails(list, bp, NULL);
  }

  /* n**2 comparison between each tails */
  for (lpa = list; lpa; lpa=lpa->next) {
    for (lpb = list; lpb; lpb=lpb->next) {
      if (lpa != lpb)
	merge_tails(lpa, lpb);
    }
  }
}

static void add_args(smacq_graph * a, smacq_graph * b) {
	int i;
	a->argv = realloc(a->argv, (a->argc + b->argc) * sizeof(char*));
	a->argv[a->argc++] = ";";
	for (i=1; i < b->argc; i++) {
		a->argv[a->argc++] = b->argv[i];
	}
}

static int merge_demuxs(smacq_graph * a, smacq_graph * b) {
	int match = 1;
	int i;

	if (!compare_elements(a,b)) return 0;

	/*
	 * For demuxs, we can only merge the nodes if 
	 * their immediate children are also identical and in same order 
	 */
	   for (i = 0; i < b->numchildren; i++) {
		if (! compare_elements(a->child[i], b->child[i])) {
			match = 0;
			break;
		}
	   }
	   if (match) {
		/* Okay, we know that a == b and a's children == b's children.
		 * So B will be completely replaced by A.
		 * Therefore, we don't need to do any recursive merging 
		 */
		return 1; /* This will tell the caller to replace refs to b with a */
	   } else {
		/* Can't merge A and B */
		return 0;
	   }
}

static int merge_vectors(smacq_graph * a, smacq_graph * b) {
	int i;

#ifndef SMACQ_OPT_VECTORS
#warning "SMACQ_OPT_VECTORS not set"
	return 0;
#endif 

	if (!a->alg.vector || !b->alg.vector || !compare_element_names(a,b))
		return 0;

	/* Move all children (including arguments) from B to A */
	/* XXX: We could look at each arg in vector and treat as set instead of bag */

	//fprintf(stderr, "vector %p added to %p\n", b, a);
	add_args(a, b);

	/* Add children while in order */
	for (i=0; i < b->numchildren; i++) {
		smacq_add_child(a, b->child[i]);
		smacq_remove_parent(b->child[i], b);
		b->child[i] = NULL;
	}
	b->numchildren = 0;

	return 1;
}

static int merge_trees(smacq_graph * a, smacq_graph * b);

static int merge_fanouts(smacq_graph * a, smacq_graph * b) {
	/*
	 * For fanout nodes, we can just make the children of A be the union of 
	 * the children of A and B.  Then get rid of B.
	 * Note that this implementation also handles the case that the two sets are identical.
	 */
	int i, j;

	if (!compare_elements(a,b)) return 0;

	/* Move children selectively */
	for (i = 0; i < b->numchildren; i++) {
		/* If any of our twins children are twins of our children, just use that child */
		for (j = 0; j < a->numchildren; j++) {
			if (merge_trees(a->child[j], b->child[i])) {
				//fprintf(stderr, "%p will be handled by %p\n", b->child[i], a->child[j]);
				b->child[i] = NULL;
				break;
			}
		}

		/* Pawn this child off on our newly found twin */
		if (b->child[i]) {
			smacq_add_child(a, b->child[i]);
			/* fprintf(stderr, "%p child %p(%s) now child of %p(%s)\n", b, 
					b->child[i], b->child[i]->name, 
					a, a->name);  */
			smacq_remove_parent(b->child[i], b);
		}
	}

	return 1;
}

static int merge_trees(smacq_graph * a, smacq_graph * b) {
	int retval;

	if (a==b)
		return 0;

	if (0 && merge_demuxs(a,b)) {
		retval = 1;
        } else if (a->alg.vector) { 
		retval = merge_vectors(a,b);
	} else {
		retval = merge_fanouts(a, b);
	}

	if (retval) {
		//smacq_destroy_graph(b);  /* Should do this, but causes errors currently */
	}

	return retval;
}

static void optimize_tree(smacq_graph * g) {
	int an, bn;
	smacq_graph * a, * b;

	if (!g) return;

	if (!g->alg.vector && !g->alg.demux) { /* Can't rewrite children of a vector */
	  for (an = 0; an < g->numchildren; an++) {
		a = g->child[an];

		for (bn=1; bn < g->numchildren; bn++) {
			b = g->child[bn];

			if (merge_trees(a, b)) {
				//fprintf(stderr, "removed %p from %p\n", b, g);
				smacq_remove_child(g, bn);
				bn--; /* remove_child will reset g->child[j] to something new */
			}
		}
	  }
	}

	for (an = 0; an < g->numchildren; an++) {
		optimize_tree(g->child[an]);
	}
}

int smacq_graphs_print(FILE * fh, smacq_graph * g, int indent) {
  int count = 0;
  smacq_graph * ap;
  for (ap = g; ap; ap=ap->next_graph) {
    count += smacq_graph_print(fh, ap, indent);
  }
  fprintf(fh, "Total number of nodes = %d\n", count);
  return count;
}

smacq_graph * smacq_merge_graphs(smacq_graph * g) {
  smacq_graph * ap, * prev;
  smacq_graph * candidate;

#ifdef SMACQ_DEBUG
  smacq_graphs_print(stderr, g, 8);
#endif

  /* Merge identical heads */
  prev = g;
  for (candidate=prev->next_graph; candidate; candidate=prev->next_graph) {
    int merged = 0;
    smacq_graph * next = candidate->next_graph;
    
    for (ap = g; ap; ap=ap->next_graph) {
      merged = merge_trees(ap, candidate);

      if (merged) {
	break;
      }
    }

    if (merged) {
      /* Remove candidate from the list */
      prev->next_graph = next;
    } else {
      /* Move down list */
      prev = prev->next_graph;
    }
  }

#ifdef SMACQ_DEBUG
  fprintf(stderr, "--- merged heads to ---\n");
  smacq_graphs_print(stderr, g, 8);
#endif

  /* Do the merge again from the tails up */
  merge_all_tails(g);

#ifdef SMACQ_DEBUG
  fprintf(stderr, "--- optimized tails to ---\n");
  smacq_graphs_print(stderr, g, 8);
#endif

  optimize_tree(g);

#ifdef SMACQ_DEBUG
  fprintf(stderr, "--- optimized trees to ---\n");
  smacq_graphs_print(stderr, g, 8);
#endif

#ifdef SMACQ_DEBUG
  fprintf(stderr, "--- final is ---\n");
  smacq_graphs_print(stderr, g, 8);
#endif
  
  return g;
}




