/*
** Copyright (C) 2000-2002 Mike Fisk <mfisk@lanl.gov>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/* $Id: substr.c,v 1.2 2003/01/19 02:07:28 mfisk Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <ctype.h>
#include "substr.h"
#include "trie.h"

#define DEBUG_SS(a)  
#define CALLOC(a) (a*)calloc(sizeof(a),1)
#define MIN(a,b) ( (a) < (b) ? (a) : (b) )
#define MAX(a,b) ( (a) > (b) ? (a) : (b) )
#define ALPHABETSIZE 256
#ifndef MAXINT
  #define MAXINT 2147483647
#endif

int totalshifts = 0;
int totalshiftby = 0;

static inline 
void * 
memdup(const void * str, size_t n)
{
    void * new = malloc(n);
    if (new) memcpy(new, str, n);

    return new;
}

static void 
bm_make_shift(unsigned char *ptrn, int m, int * realshift)
{
    int f[m+2];
    int shift[m+2];
    
    int i,j,k;

    for (i = 1; i <= m; i++) shift[i] = 2*m - i;
    j = m;
    k = m + 1;
    while (j > 0) {
  	f[j]=k;
        while ((k <= m) && (ptrn[j] != ptrn[k])) {
 		shift[k] = MIN(shift[k], m-j);
 		k = f[k];
        }
 	j--, k--;
    }
    for (i = 1; i <= k; i++) shift[i] = MIN(shift[i], m+k-i);
    j = f[k];
    while(k<=m) {
	while (k<=j) {
                /* fprintf(stderr,"shift[%i/%i] ", k, m); */
		shift[k] = MIN(shift[k], j-k+m);
		k++;
	}
	j = f[j]; 
    }
    for (i = 0; i < m; i++) { 
	realshift[i] = shift[i+1];
        /* fprintf(stderr,"shift[%i] = %i %.*s\n", i, realshift[i], m, ptrn); */
    }
    return;

}

static int 
bm_compile(substr_object * obj)
{
  int i;
  int m;
  struct pattern * p;

  for (p=obj->patterns; p; p=p->next) {
    /* Compute bad suffix table */
    p->bad = (int*)malloc(ALPHABETSIZE * sizeof(unsigned int)); 
    if (!p->bad) return 0;
    p->good = (int*)malloc(p->len * sizeof(unsigned int));
    if (!p->good) return 0;

    m = p->len;

#ifdef MSEARCH
    {
	int * sptr = &p->bad[ALPHABETSIZE];
        while (sptr-- != p->bad) *sptr = m + 1;

        i = m;
        while (i != 0) p->bad[p->pattern[i]] = i--;

    }
#else

    for (i=0; i<ALPHABETSIZE; i++) 
      p->bad[i] = m;

    for (i=0; i < m; i++) 
      p->bad[p->pattern[i]] = m - i - 1;

#endif

    bm_make_shift(p->pattern, p->len, p->good);
  }
  return 1;
}

static int
horspool_compile(substr_object * obj)
{
  int i;
  int m;
  struct pattern * p;

  for (p=obj->patterns; p; p=p->next) {
    /* Compute bad suffix table */
    p->bad = (unsigned int*)malloc(ALPHABETSIZE * sizeof(unsigned int));
    if (!p->bad) return 0;

    for (i=0; i<ALPHABETSIZE; i++) p->bad[i] = p->len;

    m = p->len - 1;
    for (i=0; i < m; i++) {
      p->bad[p->pattern[i]] = m - i;
    }
  }

  return 1;
}

#define SETMIN(a,b) if ( (a) > (b) ) { (a) = (b); } 
#define SETMAX(a,b) if ( (a) < (b) ) { (a) = (b); } 


static int 
set_compile(substr_object * obj)
{
  int i, m;
  struct pattern * p;
  unsigned char * pat;
  unsigned char c;

  if (obj->patterns) 
    obj->shortest = obj->patterns->len;

  for (p=obj->patterns; p; p=p->next)
    SETMIN( obj->shortest, p->len );

  /* Compute bad character table */
  obj->bad = (unsigned int*)malloc(ALPHABETSIZE * sizeof(unsigned int));
  if (!obj->bad) return 0;
  for (i=0; i<ALPHABETSIZE; i++) obj->bad[i] = obj->shortest;

  obj->trie = trieobj_new(TRIE_REVERSE, 0, ALPHABETSIZE);

  for (p=obj->patterns; p; p=p->next) {
    pat = memdup(p->pattern, p->len * sizeof(unsigned char));

    for (i=0; i < p->len; i++) 
	pat[i] = tolower(pat[i]);

    m = p->len - 1;
    for (i=0; i < m; i++) {
      c = pat[i];
      if ( (! obj->bad[c]) || (obj->bad[c] > m - i) )  {
	obj->bad[c] = m - i;
	obj->bad[toupper(c)] = m - i;
      }
    }

    trie_insert(obj->trie, pat, p->len, p);

    /* Don't free pat, trie_insert refers to that data */
  }

  trie_nocase(obj->trie);
  DEBUG_SS(trie_dump(obj->trie)); 

  return 1;
}

int
substr_add(substr_object * rs, int len, unsigned char * str, int nocase, void * handle, int offset, int depth) 
{
  int i;
  struct pattern * newp = calloc(sizeof(struct pattern),1);
  if (!newp) return(0);

  newp->pattern = memdup(str, len);
  if (!newp->pattern) {
    free(newp);
    return(0);
  }

  /* Convert to lower case if case-insensitive */
  if (nocase)
	for (i=0; i < len; i++) 
		newp->pattern[i] = tolower(newp->pattern[i]);

  newp->len = len;
  newp->handle = handle;
  newp->nocase = nocase;

  newp->next = rs->patterns;
  rs->patterns = newp;

  if (offset < rs->minoffset)
    rs->minoffset = offset;

  if (!depth) {
    rs->maxdepth = MAXINT;
  } else if (depth+offset > rs->maxdepth) {
    // maxdepth is measured from beginning of buffer, not offset
    rs->maxdepth = depth + offset;
  }

  return(1);
}

int 
substr_compile(substr_object * rs) 
{
  if (rs->minoffset == MAXINT) {
	  rs->minoffset = 0;
  }
  if (rs->maxdepth == -1) {
	  rs->maxdepth = 0;
  }
  return(rs->compile(rs));
}

/*
 * Search for pattern p in test from hay to stop (inclusive)
 * Start with initial endpoint of shay (initially should be p->len-1)
 */
static inline int 
bm_match(struct pattern * p, unsigned char * hay, unsigned char * hp, unsigned char * stop)
{
  unsigned char * pp, * pattern = p->pattern;
  unsigned char * ppinit = pattern + p->len - 1;

  while (hp <= stop) {
    pp = ppinit;
    while (*pp == *hp) {
	pp--, hp--;

        if (pp < pattern)  /* Found */
           return (hp - hay + 1);
    }

    hp += MAX(p->bad[*hp], p->good[pp - pattern]);  
  }

  return -1;
  
}

static inline int 
bm_match_nocase(struct pattern * p, unsigned char * hay, 
		unsigned char * hp, unsigned char * stop)
{
  unsigned char * pp, * pattern = p->pattern;
  unsigned char * ppinit = pattern + p->len - 1;

  while (hp <= stop) {
    pp = ppinit;
    while (*pp == (unsigned char)tolower(*hp)) {
	pp--, hp--;

        if (pp < pattern)  /* Found */
           return (hp - hay + 1);
    }

    hp += MAX(p->bad[(unsigned char)tolower(*hp)], p->good[pp - pattern]);  
  }

  return -1;
  
}

/*
 * Search for pattern p in test from hay to stop (inclusive)
 * Start with initial endpoint of shay (initially should be p->len-1)
 */
static inline int 
horspool_match(struct pattern * p, unsigned char * hay, 
	       unsigned char * shay, unsigned char * stop)
{
  unsigned char * pp, * hp, * pattern = p->pattern;
  int m = p->len-1;
  unsigned char * ppinit = pattern + m;


  while (shay <= stop) {
    pp = ppinit;
    hp = shay; 
    while (1) {
        if (*pp != *hp) break;

	pp--, hp--;

        if (pp < pattern)  /* Found */
           return (shay - hay - m);
    }

    shay += p->bad[*shay];
  }

  return -1;
  
}

/* 
 * Serch for pattern string in the haystack.
 * Move pattern from left ot right until right edge (shay) passes stop.
 * Returns offset within hay to beginning of pattern string.
 */
static inline int 
horspool_match_nocase(struct pattern * p, unsigned char * hay, 
	       unsigned char * shay, unsigned char * stop)
{
  unsigned char * pp, * hp, * pattern = p->pattern;
  int m = p->len-1;
  unsigned char c, shayc;

  while (shay <= stop) {
    pp = pattern + m;
    hp = shay; 
    shayc = c = tolower(*hp);
    while (*pp == c) {
      pp--, c = tolower(*(--hp));

        if (pp < pattern)  /* Found */
	  return (shay - hay - m);
    }

    shay += p->bad[shayc];
  }

  return -1;
}


static int
bm_search(substr_object * obj, unsigned char * hay, int len,
	  struct substr_search_result * stat)
{
  struct pattern * p = stat->p;
  char * startp;
  int shift = stat->resume.left;
  len--;
  if (len < 0) return 0;

  if (p) {
        startp = shift + hay;
  } else {
	p = obj->patterns;
        startp = hay + p->len - 1;
  }

  while(1) {
    if (p->nocase) 
    	shift = bm_match_nocase(p, hay, startp, hay+len);
    else
    	shift = bm_match(p, hay, startp, hay+len);

    if (shift != -1) { 
         stat->shift = shift;
         stat->p = p;
         stat->resume.left = shift + p->len;
         return 1;
    }

    p = p->next;
    if (!p) return 0;
    startp = hay + p->len - 1;
  } 
      
}

static int
horspool_search(substr_object * obj, unsigned char * hay, int len,
	  struct substr_search_result * stat)
{
  struct pattern * p = stat->p;
  char * startp;
  int shift = stat->resume.left;
  len--;
  if (len < 0) return 0;

  if (p) {
    startp = hay + shift;
  } else {
    p = obj->patterns;
    startp = hay + p->len - 1;
  }

  while(1) {
    if (p->nocase) 
    	shift = horspool_match_nocase(p, hay, startp, hay+len);
    else
    	shift = horspool_match(p, hay, startp, hay+len);

    if (shift != -1) { 
        stat->shift = shift;
        stat->p = p;
        stat->resume.left = shift + p->len;

        return 1;
    }
    p = p->next;
    if (!p) return 0;
    startp = hay + p->len - 1;
  } 
      
  return 0;
}

#define NEWEND2 (t = mtries[*(hp = (endp += bad[*endp]))])
#define NEWEND endp+=bad[*endp]; hp = endp; t=mtries[*hp];

/*
 * This search uses Setwise Boyer-Moore-Horspool to search for any of the
 * strings defined in @obj in the string @hay of length @len.  To start
 * a new search, set @stat->resume.trie = NULL
 *
 * The return value is 1 if there is a match, and 0 if not.
 * If there is a match, @stat is made to reference it.
 *
 * If there is a match, there may be additional matches than can be found
 * by restarting the search with with @stat preserved.  When the return value
 * is 0, there are no more possible matches.
 */
static int
set_search(substr_object * obj, unsigned char * hay, int len,
	   struct substr_search_result * stat)
{
  unsigned char * hp, * endp, * stop;
  struct trie ** mtries;
  struct trie * t;
  unsigned int * bad = obj->bad;
  struct pattern * p;

  if (len <= 0) return 0;

  stop = hay + len - 1;
  mtries = obj->trie->mtries;

  if ((hp = endp = stat->resume.endp)) {
  	t = stat->resume.trie;
  } else {
  	hp = endp = hay + obj->shortest - 1;
  	t = mtries[*endp];
  }

  while (endp <= stop) {
	if (!t) {
		NEWEND;
        } else {
		hp--;

		if (t->target) { /* Success */
		    p = t->target;
		    if (p->nocase || !memcmp(p->pattern, hp + 1, p->len)) {

			stat->shift = hp - hay + 1;
			stat->p = p;
			if (hp < hay) {
				NEWEND;
	        	} else {
				t = t->branch[*hp];
			}
			/* added cast -- cmg */
			stat->resume.endp = endp;
			stat->resume.trie = t; 
			return 1;
                    }
		}

		if (hp < hay) {
			NEWEND;
	        } else {
			t = t->branch[*hp];
		}
	}
  }

  return 0;
}

/*
 * This search uses Aho-Corasick to search for any of the
 * strings defined in @obj in the string @hay of length @len.  To start
 * a new search, set @stat->resume.trie = NULL
 *
 * The return value is 1 if there is a match, and 0 if not.
 * If there is a match, @stat is made to reference it.
 *
 * If there is a match, there may be additional matches than can be found
 * by restarting the search with with @stat preserved.  When the return value
 * is 0, there are no more possible matches.
 */
static int
ac_search(substr_object * obj, unsigned char * hay, int len,
	   struct substr_search_result * stat)
{
  unsigned char * hp, * stop;
  struct trie ** mtries;
  struct trie * t;
  struct pattern * p;

  if (len <= 0) return 0;

  stop = hay + len;
  mtries = obj->trie->mtries;

  if ((hp = stat->resume.endp)) {
	if (hp >= stop) return 0;

  	t = stat->resume.trie;
  } else {
  	hp = hay;
	t = mtries[tolower(*hp++)];
  }

  while (hp <= stop) {
	if (!t) {
		t = mtries[*(hp++)];
		continue;
	}

	if (t->target) {
		p = (struct pattern*)t->target;
		stat->shift = hp - hay - p->len;
		if (p->nocase || !memcmp(p->pattern, hay + stat->shift, p->len)) {

			stat->p = t->target;

			if (t->branch[*hp]) 
               			t = t->branch[*(hp++)];
			else
                		t = t->fail;

			/* added cast -- cmg */
			stat->resume.endp =  hp;
			stat->resume.trie = t; 
			return 1;
		}
	}

	if (t->branch[*hp]) 
		t = t->branch[*(hp++)];
	else 
		t = t->fail;
  }

  return 0;
}

static int 
ac_failure(substr_object * obj, struct trie * parent, struct trie * child, unsigned char c, int depth) 
{
	if (depth == 0) { /* Work on this entry */
	  struct trie * f = NULL;

	  if (parent) {
		f = parent->fail;
		while (f) {
		 	if (f->branch[c]) {
				child->fail = f->branch[c];
				return(1);
                        }
			f = f->fail;
                }
	  } 

	  /* No failure path */
   	  child->fail = obj->trie->mtries[c];
          if (child->fail == child) 
		child->fail = NULL;

   	  return(1); /* There could be more (we terminated because of depth rather than exahustion of trie */

        } else { /* Go deeper */
	  int more = 0;
	  unsigned long ch;
	  for (ch = 0; ch < 0xff; ch++) {
		if (child->branch[ch]) {
			more += ac_failure(obj, child, child->branch[ch], ch, depth - 1);
		} 
	  }

	  return(more);
	}

}

static int 
ac_compile(substr_object * obj)
{
  struct pattern * p;
  int i;

  /* Discover shortest pattern length */
  if (obj->patterns) 
    obj->shortest = obj->patterns->len;

  for (p=obj->patterns; p; p=p->next)
    SETMIN( obj->shortest, p->len );

  /* Build Trie */
  obj->trie = trieobj_new(TRIE_FORWARD, 0, ALPHABETSIZE);
  for (p=obj->patterns; p; p=p->next) {
    unsigned char * pat = memdup(p->pattern, p->len * sizeof(unsigned char));
    for (i=0; i < p->len; i++)
	pat[i] = tolower(pat[i]);

    trie_insert(obj->trie, pat, p->len, p);

    /* Don't free pat, trie_insert refers to that data */
  }

  /* Compute failure function */
  
  for (i = 0; ; i++) {
	int more = 0;
	unsigned long ch;
	for (ch = '\0'; ch < 0xff; ch++) {
		struct trie * t;
		if ((t = obj->trie->mtries[ch])) {
			more += ac_failure(obj, NULL, t, ch, i);
		}
        }
	if (!more) break;
  }

  trie_nocase(obj->trie);
  DEBUG_SS(trie_dump(obj->trie)); 

  return 1;
}
			
static int
fast_compile(substr_object * set)
{
  struct pattern * p;
  int setsize=0;

  for (p=set->patterns; p; p=p->next) 
    setsize++;

  /* Choose algorithm based on size */
  if (setsize <= 2) { /* Use Horspool */
    set->search = &horspool_search;
    set->compile = &horspool_compile;
  } else if (setsize < 101) { /* Use Aho-Corasick */
    set->search = &ac_search;
    set->compile = &ac_compile;
  } else {
    set->search = &set_search;
    set->compile = &set_compile;
  }

  return set->compile(set);
}

substr_object *
substr_new(enum substr_type searchtype)
{
  substr_object * rs = CALLOC(substr_object);
  if (!rs) return 0;

  rs->patterns = NULL;
  rs->searchtype = searchtype;

  if (searchtype == SUBSTR_SETBMH) {
    rs->search = &set_search;
    rs->compile = &set_compile;
  } else if (searchtype == SUBSTR_HORSPOOL) { /* HORSPOOL */
    rs->search = &horspool_search;
    rs->compile = &horspool_compile;
  } else if (searchtype == SUBSTR_AC) { /* Aho-Corasick */
    rs->search = &ac_search;
    rs->compile = &ac_compile;
  } else if (searchtype == SUBSTR_BM) { /* Boyer-Moore */
    rs->search = &bm_search;
    rs->compile = &bm_compile;
  } else if (searchtype == SUBSTR_FAST) { /* Boyer-Moore */
    rs->search = NULL;
    rs->compile = &fast_compile;
  } else {
    return NULL;
  }

  rs->minoffset = MAXINT;
  rs->maxdepth = -1;

  return(rs);
}

#ifdef MAIN

/* Main function for testing this library in isolation */
int
main(int argc, char ** argv) {
  int len;
  int niter;
  int found = 0;
  int numalg = 6;
  int i, j;
  int handle = 0;
  unsigned char * pat;
  substr_object * search[numalg];
  unsigned char buf[8192];
  struct timeval tv;
  int alg;
  unsigned char * aname[numalg];
  int correct = 0;
#ifdef SUBSTR_PERFMON
  int totalh[numalg], totall[numalg];
#endif

  aname[SUBSTR_HORSPOOL] = "BMH";
  aname[SUBSTR_AC] = "A-C";
  aname[SUBSTR_SETBMH] = "Set";
  aname[SUBSTR_BM] = "B-M";
  aname[SUBSTR_FAST] = "Auto";

  if (argc != 3) {
	fprintf(stderr, "Usage: %s len niter\n\n", argv[0]);
        fprintf(stderr, "Generate a random string of length len and compares it to a list of input\nsearch strings on stdin (one per line).\n");
        fprintf(stderr, "Do this niter times.\n\n");
        exit(1);
  }
  len = atoi(argv[1]);
  niter = atoi(argv[2]);

  for (alg=0; alg<numalg; alg++)  {
     search[alg] = substr_new(alg);
#ifdef SUBSTR_PERFMON
     totall[alg] = totalh[alg] = 0;
#endif
  }

  while ((pat = fgets(buf, 8192, stdin))) {
    int l;
    int in = 0;

    buf[strlen(buf)-1] = '\0';  /* Chop newline */

    l = strlen(buf);
    
    for(i=0, j=0; i < l; i++) {
      if (buf[i] == '|') 
	in = !in;
      else if (!in) {
	buf[j] = buf[i];
	j++;
	continue;
      } else if (buf[i] == ' ')
	continue;
      else {
	unsigned int c;
	unsigned char num[3];

	memcpy(num, buf+i, 2);
	num[2] = '\0';

	if (sscanf(num, "%x", &c)) {
	  buf[j] = (short)c;
/* 	  fprintf(stderr, "Checking next two unsigned chars, %s: %c: %s\n", num, buf[j], buf); */
	  j++;
	  i++; /* Consume two chacaters */
	} else {
	  fprintf(stderr,"Non hex-input in %s at %s\n", buf, buf+i);
	}
      }
    }

    for (alg=0; alg<numalg; alg++) 
    	substr_add(search[alg], j, buf, 0, (void*)handle);
    handle++;
  }

  for (alg=0; alg<numalg; alg++) 
     substr_compile(search[alg]);

  gettimeofday(&tv, NULL);

  /* srandom(tv.tv_usec);   */
  srandom(3);   

  for (i = 0; i<niter; i++) {
    struct substr_search_result res;
    unsigned char str[len];
    int ret;
    unsigned long low, high, eh, el;


    for (j = 0; j<len; j++) 
      str[j] = (unsigned char)((random()+i) % 256); 
      /* str[j] = 'a'+((random()+i) % 26); */
      /* str[j] = ((random()+i) % 26) + ((random()+i) % 2 ? 'a' : 'A'); */

    DEBUG_SS(printf("\nFind %.*s\n", len, str)); 

    for (alg=0; alg<numalg; alg++) {
       memset(&res, 0, sizeof(res));
       found = 0;

#ifdef SUBSTR_PERFMON
       rdtsc(low,high);
#endif
       do {
         ret = substr_search(search[alg], str, len, &res);
         if (ret) {
/* 
	   fprintf(stderr, "%s %.*s found %p %.*s at %.*s\n", aname[alg],  
		len, str,
		res.p->handle, 
		res.p->len, res.p->pattern, 
		len - res.shift, str+res.shift);
*/
	   found++;
         }
       } while (ret);
#ifdef SUBSTR_PERFMON
       rdtsc(el,eh);
     
       totall[alg] += el - low;
       totalh[alg] += eh - high;
#endif

       if (alg == 0) {
          fprintf(stderr, "\n%d matches\n", found);
          correct = found;
       } else if (correct != found) 
	  fprintf(stderr, "%s found %d matches instead of %d \n", 
                 aname[alg], found, correct);

       fprintf(stderr, "%s Time %ld,%ld ; %d matches\n", aname[alg], eh-high, el-low, found);
    }
  }

#ifdef SUBSTR_PERFMON
  for (alg=0; alg<numalg; alg++) 
	fprintf(stderr, "%s Time %ld,%ld\n" , aname[alg], totalh[alg], totall[alg]);
#endif

  return 0;
}
#endif
