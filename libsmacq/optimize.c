#include <smacq-internal.h>

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

static int compare_trees(smacq_graph * a, smacq_graph *b) {
  int i, j;

  if (a==b) return 1;

  if (a->argc != b->argc) return 0;

  for (i=0; i < a->argc; i++) {
    if (strcmp(a->argv[i], b->argv[i])) return 0;
  }

  //fprintf(stderr, "%p(%s) === %p(%s)\n", a, a->name, b, b->name);

  /* Compare children at this level */
  for (i = 0; i < b->numchildren; i++) {
    for (j = 0; j < a->numchildren; j++) {
      if (!compare_trees(a->child[j], b->child[i])) {
	return 0;
      }
    }
  }

  return 1;
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

static int compare_tail_ends(smacq_graph * a, smacq_graph * b) {
  if (!a->numchildren) {
    return compare_elements(a,b);
  }

  if (compare_tail_ends(a->child[0], b->child[0])) {
    if (compare_elements(a,b)) {
      return 1;
    } else {
      /* This is the top of the commonality */
      smacq_remove_child(b, 0);
      smacq_add_child(b, a->child[0]);
      smacq_add_parent(a->child[0], b);

      return 0;
    }
  } else {
    return 0;
  }
}

static void compare_tails(struct list * alist, struct list * blist) {
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

  if (compare_tail_ends(a, b)) {
    assert(blist->parent); /* else this would be a head */
    smacq_remove_child(blist->parent, 0);
    smacq_add_child(blist->parent, a);
    smacq_add_parent(a, blist->parent);
    
    if (blist->next) {
      /* Remove b tail from list of tails */
      struct list * lp = blist->next;
      *blist = *lp;
      free(lp);
    }
  }
}

static void merge_tails(smacq_graph * g) {
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
	compare_tails(lpa, lpb);
    }
  }
}

static int merge_tree(smacq_graph * a, smacq_graph *b) {
	int i, j;

	if (a==b) return 0;
	if (a->argc != b->argc) return 0;

	for (i=0; i < a->argc; i++) {
	  if (strcmp(a->argv[i], b->argv[i])) return 0;
	}

	// fprintf(stderr, "%p(%s) === %p(%s)\n", a, a->name, b, b->name);

	for (i = 0; i < b->numchildren; i++) {
		/* If any of our twins children are twins of our children, just use that child */
		for (j = 0; j < a->numchildren; j++) {
			if (merge_tree(a->child[j], b->child[i])) {
				b->child[i] = NULL;
				break;
			}
		}

		/* Pawn this child off on our newly found twin */
		if (b->child[i]) {
			smacq_add_child(a, b->child[i]);
			smacq_add_parent(b->child[i], a);
			/* fprintf(stderr, "%p child %p(%s) now child of %p(%s)\n", b, 
					b->child[i], b->child[i]->name, 
					a, a->name); */
			smacq_remove_parent(b->child[i], b);
		}
	}

	return 1;
}

smacq_graph * smacq_merge_graphs(smacq_graph * g) {
  smacq_graph * ap, * bp;
  smacq_graph * candidate;

#ifdef SMACQ_DEBUG
  for (bp = g; bp; bp=bp->next_graph) {
    smacq_graph_print(stderr, bp, 8);
  }
#endif

  /* Merge identical heads */
  for (bp = g, candidate=bp->next_graph; candidate; candidate = bp->next_graph) {
    int merged = 0;
    
    for (ap = g; ap; ap=ap->next_graph) {
      merged = merge_tree(ap, candidate);

      if (merged) {
	break;
      }
    }

    if (merged) {
      /* Remove candidate from the list */
      bp->next_graph = candidate->next_graph;
    } else {
      /* Move down list */
      bp = bp->next_graph;
    }
  }

  /* Do the merge again from the tails up */
  merge_tails(g);

#ifdef SMACQ_DEBUG
  fprintf(stderr, "--- optimized to ---\n");
  for (bp = g; bp; bp=bp->next_graph) {
    smacq_graph_print(stderr, bp, 8);
  }
#endif
  
  return g;
}

