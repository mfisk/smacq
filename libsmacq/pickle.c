#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pickle.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

// XXX: Should use buffered IO

struct sockdatum {
  dts_object datum;
  int namelen;  
};

struct pickle {
  GArray * source;
};

static int receive_it(int fd, void * space, int size) {
  int current;
  int total = 0;
  while (1) {
    current = recv(fd, space+total, size - total,0);
    if (current <= 0) {
      if (current < 0) {
	if (errno == EINTR) continue;
	perror("recv");
      }
      if (!current && !total) return 0;
      return -1;
    }
    total += current;
    if (total >= size) return size;
  }
}

static int send_it(int fd, void * space, int size) {
  int current;
  int total = 0;
  while (1) {
    current = write(fd, space+total, size - total);
    if (current <= 0) {
      if (current < 0) 
	if (errno == EINTR) continue;
      if (!current && !total) return 0;
      perror("write");
      return -1;
    }
    total += current;
    if (total >= size) return size;
  }
}

struct pickle * pickle_init(void) {
  struct pickle * pickle = g_new(struct pickle, 1);
  pickle->source = g_array_new(TRUE, TRUE, sizeof(GArray *));

  return(pickle);
}

static GArray ** getsmap(struct pickle * pickle, int fd) {
  if (fd > pickle->source->len) 
    pickle->source = g_array_set_size(pickle->source, fd+1);

  return(&g_array_index(pickle->source, GArray*, fd));
}

void pickle_close_source(struct pickle * pickle, int fd) {
  GArray ** smap = getsmap(pickle, fd);

  if (*smap)
    g_array_free(*smap, FALSE);

  *smap  = g_array_new(TRUE, TRUE, sizeof(int)); // XXX
}

static int * gettype(struct pickle * pickle, int fd, int type)  {
  GArray ** smap = getsmap(pickle, fd);

  if (!(*smap)) 
    *smap  = g_array_new(TRUE, TRUE, sizeof(int)); // XXX
  
  if (type > (*smap)->len)
    *smap = g_array_set_size(*smap, type+1);
  
  return (&g_array_index((*smap), int, type));
}

static int maptype(struct pickle * pickle, int type, int fd)  {
  int * btype  = gettype(pickle, fd, type);
  //fprintf(stderr, "%d on %d maps to type %d\n", type, fd, *btype);
  assert(*btype);
  return(*btype);
}

static int addnewtype(smacq_environment * env, char * name, int extnum, struct pickle * pickle, int fd) {
  int * typep = gettype(pickle, fd, extnum);

  if (! *typep) 
      *typep = smacq_requiretype(env, name);

  return *typep;
}

static int getnewtype(smacq_environment * env, int fd, 
	       struct sockdatum * hdr, struct pickle * pickle) {
  char *name;
  int temp;
  
  name = calloc(hdr->namelen + 1,1);
  if ((temp = receive_it(fd, name, hdr->namelen)) <= 0) {   
    if (temp < 0) fprintf(stderr,"Error receiving new type, closing client\n");
    else fprintf(stderr,"Error: New type, no typename sent, closing client\n");
    return -1;
  }

  //fprintf(stderr,"New type %d -> %s, %d, %d\n", hdr->datum.type, name, env->typenum_byname("packet"), strcmp("packet", name));
  

  addnewtype(env, name, dts_gettype(&hdr->datum), pickle, fd);
  free(name);
  return 0;
}

/* 0 is EOF, -1 is error midstream */
int read_datum(smacq_environment * env, struct pickle * pickle, int fd, const dts_object ** datump) {
  struct sockdatum  hdr;
  int temp = receive_it(fd, &hdr, sizeof(struct sockdatum));

  //fprintf(stderr, "recvd datum of type %d, size %d\n", dts_gettype(&hdr.datum), dts_getsize(&hdr.datum));

  if (temp <= 0) return temp;
  
  if (hdr.namelen)  {
    if (getnewtype(env, fd, &hdr, pickle) < 0) 
      return -1;
  }

  assert(dts_getsize(&hdr.datum) + sizeof(struct sockdatum) > 0);
  *datump = (dts_object*)smacq_alloc(env, dts_getsize(&hdr.datum) + sizeof(dts_object),
					 maptype(pickle, dts_gettype(&hdr.datum), fd));
  
  if ((temp = receive_it(fd, dts_getdata(*datump), dts_getsize(&hdr.datum))) < 0) {
      free((void*)*datump);
      return -1;
  }

  return 1;
}

/* 0 is EOF, -1 is error midstream */
int write_datum(smacq_environment * env, struct pickle * pickle, int fd, const dts_object * datum) {
  struct sockdatum hdr;
  int * typep = gettype(pickle, fd, dts_gettype(datum));
  char * name = NULL;
    
  memcpy(&hdr.datum, datum, sizeof(dts_object));
  hdr.namelen = 0;

  if (! *typep) {
    *typep = 1;

    name=dts_typename_bynum(env, dts_gettype(&hdr.datum));
    hdr.namelen=strlen(name);
    //fprintf(stderr, "Sending new type %d -> %s\n", dts_gettype(&hdr.datum), name);
  } else {
    //name=env->typename_bynum(dts_gettype(&hdr.datum));
    //fprintf(stderr, "Sending old type %d -> %s\n", dts_gettype(&hdr.datum), name);
  }

  if (1 > send_it(fd, &hdr, sizeof(struct sockdatum)))  
    return -1;
  if (hdr.namelen && (1 > send_it(fd, name, hdr.namelen)))
    return -1;
  if (1 > send_it(fd, dts_getdata(datum), dts_getsize(datum)))  
    return -1;

  return 1;
}
