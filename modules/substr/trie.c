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
#include <stdio.h> 
#include <stdlib.h> 
#include <ctype.h> 
#include "trie.h"

#define DEBUG_TRIE(a) 
#define MALLOC(a) (a*)calloc(sizeof(a),1)
#ifndef min
#define min(a,b) ( (a) < (b) ? (a) : (b) )
#endif /* !min */

struct trie *
trie_new (trieobj * self, void * target, unsigned char * str, int len)
{
  struct trie * newtrie;

  newtrie = MALLOC(struct trie);
  if (!newtrie) {return NULL;}

  newtrie->branch = (struct trie**)calloc(self->alphabet * sizeof(struct trie*), 1);
  if (!newtrie->branch) {return NULL;}


  if (self->collapse) {
  	newtrie->val = str;
  	newtrie->len = len;
  	newtrie->target = target;
  } else {
	newtrie->len = 0;
	if (len) {
           if (self->direction == TRIE_FORWARD) 
              newtrie->branch[*str] = trie_new(self, target, str+1, len-1);
           else 
              newtrie->branch[str[len-1]] = trie_new(self, target, str, len-1);
	} else {
	   newtrie->target = target;
        }
  }
      
  return newtrie;
}

/* 
 * Split the specified node into two nodes.  The current node will become the parent
 * and contain @keep characters.  The middle character will be used for a new 
 * branch entry and the child will get the remainder of the characters.
 */
int
trie_split (trieobj * self, struct trie * trie, int keep)
{
  /* Split this node */
  struct trie * newtrie = MALLOC(struct trie);
  if (!newtrie) {return 0;}

  if (self->direction == TRIE_FORWARD) {
    newtrie->branch = (struct trie**)malloc(self->alphabet * sizeof(struct trie*));
    if (!newtrie->branch) {return 0;}

    /* The following may set len=0, but val != NULL */
    newtrie->val = trie->val + keep + 1;
    newtrie->len = trie->len - keep - 1;

    trie->len = keep;
    trie->branch[trie->val[keep]] = newtrie;

    return 1;
  } else {
    *newtrie = *trie;
     
    trie->branch = (struct trie**)malloc(self->alphabet * sizeof(struct trie*));
    if (!trie->branch) {return 0;}

    newtrie->len = newtrie->len - keep - 1;

    trie->val = trie->val + trie->len - keep;
    trie->len = keep;
    trie->target = NULL;
    trie->branch[trie->val[-1]] = newtrie;

    DEBUG_TRIE(
	printf("SPLIT %.*s %d now child of %*s\n", trie->len, trie->val, trie->len,
		      newtrie->len, newtrie->val);
        fprintf(stderr,"newtrie len is %d, was %d, keep %d\n", newtrie->len, newtrie->len +1 + keep, keep);
        fprintf(stderr,"trie len is %d, was %d, keep %d\n", trie->len, newtrie->len  +1 + keep, keep);
    )
    
    return 1;
  }
}

/*
 * Search the trie for matching prefixes of the string @hay of length @len.  
 *
 * The result @left is the number of characters in @len that weren't matched.
 * The result @trie is a pointer to the trie of the longest matching prefix.
 * The return value is a pointer to the target if there was a matching prefix with
 *                  a target defined (with trie_insert()).
 *
 * To initiate a new search, use a NULL value for @trie and @left=@len
 *
 * If the return value is nonzero, there may be additional matches and the function 
 * should be called again with @left and @trie preserved between invocations.
 *
 * If the trie is compacted (trie->len != 0), the trie's string (trie->val) 
 * matches and has been deducted from @left.
 */
/* static inline  */
void *
trie_find(trieobj * self, unsigned char * hay, int len, int * leftp, struct trie ** triep)
{
  unsigned char * start, * common, * tvp, * tvend;
  struct trie * t;
  int left = *leftp;
  struct trie * trie = *triep;
  int tl;

  while(left) {
    if (self->direction == TRIE_FORWARD) {
      start = hay + len - left;
    } else {
      start = hay + left - 1;
    }

    if (! trie) {
        t = self->mtries[*start];
    } else {
	t = (trie)->branch[*start];
    }

    DEBUG_TRIE(fprintf(stderr,"Transition from %x to %x on %c\n", trie, t, start));
    if (!t) break;

    if ((tl = t->len)) { /* Must match trie->val too */
          if (tl > left) break;

    	  if (self->direction == TRIE_FORWARD) {
    	    common = start;
    	  } else {
    	    common = start - tl;
    	  }
          DEBUG_TRIE(fprintf(stderr,"Check (%.*s) vs. (%.*s)\n", t->len, t->val, t->len, common));

          for(tvp = t->val, tvend = tvp + tl; tvp < tvend; common++, tvp++)
	     if (*tvp != *common) goto end;

          /* Matched this node */
          left -= tl;
    } 
    DEBUG_TRIE(fprintf(stderr,"Transition2 from %x to %x on %c\n", trie, t, start));

    /* Matched this node */
    left--;

    if (t->target) { 	/* This is an accepting node */
       *leftp = left;
       *triep = t;
       return(t->target);
    }

    trie = t;
  }

end:
  *leftp = left;
  *triep = trie;
  return(NULL);
}


int
trie_insert(trieobj * self, unsigned char * str, int len, void * target)
{
  struct trie * trie = NULL;
  int left = len;
  void * match;
  int i;

  if (len < 1) return 0;

  DEBUG_TRIE(fprintf(stderr,"Insert %s\n",str));
  while(1) {
  	match = trie_find(self, str, len, &left, &trie);
	if (match) {
	     DEBUG_TRIE(fprintf(stderr,"not correct 4!\n"));
	  if (!left) {
	  	/* Already a terminating node */
          	/* Ignoring this new target.  XXX should keep list. */
	  	return(1);
          } else {
 		/* Haven't found whole string*/
		continue;
	  }
  	} else if (!left) {
                /* Make this node terminate */
                /* Assertion: trie must be nonzero here or we wouldn't have matched any */
                trie->target = target;
		return(1);
        } else { /* Must add somehow */
	  struct trie ** ary;

	  DEBUG_TRIE(fprintf(stderr,"correct 2! %x\n",trie));

          if (!trie) 
		ary = self->mtries;
          else 
		ary = trie->branch;

 	  if (self->direction == TRIE_FORWARD)  
		trie = ary[str[len - left]];
          else 
		trie = ary[str[left - 1]];

          if (trie) { /* Must split */
	     int common;
             /* Assertion: trie->len must be nonzero or trie_find would have left us here */

	     left--; /* Consumed 1 to get here */
             common = min(trie->len, left);

	     DEBUG_TRIE(fprintf(stderr,"correct!\n"));
             if (self->direction == TRIE_REVERSE) {
	        for (i=1; i <= common; i++) 
			if (trie->val[trie->len - i] != str[left - i]) { break; }
		i--;
   	     } else {
	        for (i=0; i < common; i++) 
			if (trie->val[i] != str[i]) break;
	     }
	     DEBUG_TRIE(
			if (i == trie->len) 
			fprintf(stderr, "splitall! %d, %d, %d\n", i, left, trie->len);
			fprintf(stderr,"Split1 %s keep %d of %d\n", trie->val, i, trie->len);
             );
	     
	     trie_split(self, trie, i);
	     DEBUG_TRIE(fprintf(stderr,"Split2 %s keep %d\n", trie->val, i));
	     left -= i;

	     continue;
	     ary = trie->branch; 
          }

	  /* Graft onto trie */
	  if (self->direction == TRIE_FORWARD) {
                ary[str[len-left]] = trie_new(self, target, str+len-(left-1), left-1); 
	  } else {
	        ary[str[left-1]] = trie_new(self, target, str, left-1);
          }
          return 1;
        }
  }
}

void
trie_nocase_branch(trieobj * self, struct trie * t)
{
	unsigned long ch;

        for (ch = 0; ch < self->alphabet; ch++) {   
	   if (!t->branch[ch]) continue;

	   t->branch[toupper(ch)] = t->branch[ch];
	   t->branch[tolower(ch)] = t->branch[ch];
	   trie_nocase_branch(self, t->branch[ch]);
        }
}

void
trie_nocase(trieobj * self)
{
	unsigned long ch;

        for (ch = 0; ch < self->alphabet; ch++) {   
	   if (!self->mtries[ch]) continue;
	   self->mtries[toupper(ch)] = self->mtries[ch];
	   self->mtries[tolower(ch)] = self->mtries[ch];
	   trie_nocase_branch(self, self->mtries[ch]);
        }
}

#include <string.h>
void
trie_dump_branch(trieobj * self, struct trie * t, char * path, int plen)
{
	unsigned long ch;

	if (t->target)
	printf("\t%p target is %s\n", t, ((struct pattern*)t->target)->pattern);
	printf("\t%p table at %p fails to %p\n", t, t->branch, t->fail);
	printf("\t%p val is %.*s\n", t, t->len, t->val);

	path = (char*)realloc(path, plen + 1 + t->len);
	memcpy(path+plen, t->val, t->len); 
	plen += 1 + t->len; 

        for (ch = 0; ch < self->alphabet; ch++) {   
	   if (!t->branch[ch]) continue;

	   path[plen-1] = ch;

	   printf("[%.*s] -> %p\n", plen, path, t->branch[ch]);
	   trie_dump_branch(self, t->branch[ch], path, plen);
        }
}
void
trie_dump(trieobj * self)
{
	unsigned long ch;
	char * str = (char*)malloc(256);

        for (ch = 0; ch < self->alphabet; ch++) {   
	   if (!self->mtries[ch]) continue;
	   printf("Root [%ld] -> %p\n", ch, self->mtries[ch]);
	   str[0] = ch;
	   trie_dump_branch(self, self->mtries[ch], str, 1);
        }
}

trieobj * trieobj_new(direction, collapse, alphabet)
{
   trieobj * t = MALLOC(trieobj);
   if (!t) return 0;
   t->direction = direction;
   t->collapse = collapse;
   t->alphabet = alphabet;
   t->mtries = (struct trie**)calloc(alphabet * sizeof(struct trie*), 1);

   if (!t->mtries) return 0;

   return t;
}

  
