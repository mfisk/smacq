#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pickle.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <dts.h>
#include <DtsObject.h>

// XXX: Should use buffered IO

struct sockdatum {
  DtsObject datum;
  int namelen;  
};

struct pickle {
  GArray * source;
};

static int receive_it(int fd, unsigned char * space, int size) {
  int current;
  int total = 0;
  while (1) {
    current = recv(fd, space + total, size - total,0);
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

static int send_it(int fd, unsigned char * space, int size) {
  int current;
  int total = 0;
  while (1) {
    current = write(fd, space + total, size - total);
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
  if (fd > (int)pickle->source->len) 
    pickle->source = g_array_set_size(pickle->source, fd+1);

  return(&g_array_index(pickle->source, GArray*, fd));
}

void pickle_close_source(struct pickle * pickle, int fd) {
  GArray ** smap = getsmap(pickle, fd);

  if (*smap)
    g_array_free(*smap, FALSE);

  *smap  = g_array_new(TRUE, TRUE, sizeof(int)); // XXX
}

static int * get_type(struct pickle * pickle, int fd, unsigned int type)  {
  GArray ** smap = getsmap(pickle, fd);

  if (!(*smap)) 
    *smap  = g_array_new(TRUE, TRUE, sizeof(int)); // XXX
  
  if (type > (*smap)->len)
    *smap = g_array_set_size(*smap, type+1);
  
  return (&g_array_index((*smap), int, type));
}

static int maptype(struct pickle * pickle, unsigned int type, int fd)  {
  int * btype  = get_type(pickle, fd, type);
  //fprintf(stderr, "%d on %d maps to type %d\n", type, fd, *btype);
  assert(*btype);
  return(*btype);
}

int DTS::pickle_addnewtype(char * name, unsigned int extnum, struct pickle * pickle, int fd) {
  int * typep = get_type(pickle, fd, extnum);

  if (! *typep) 
      *typep = requiretype(name);

  return *typep;
}

int DTS::pickle_getnewtype(int fd, struct sockdatum * hdr, struct pickle * pickle) {
  unsigned char *name;
  int temp;
  
  name = (unsigned char*)calloc(hdr->namelen + 1,1);
  if ((temp = receive_it(fd, name, hdr->namelen)) <= 0) {   
    if (temp < 0) fprintf(stderr,"Error receiving new type, closing client\n");
    else fprintf(stderr,"Error: New type, no typename sent, closing client\n");
    return -1;
  }

  //fprintf(stderr,"New type %d -> %s, %d, %d\n", hdr->datum.type, name, env->typenum_byname("packet"), strcmp("packet", name));
  

  pickle_addnewtype((char*)name, hdr->datum->gettype(), pickle, fd);
  free(name);
  return 0;
}

/* 0 is EOF, -1 is error midstream */
DtsObject DTS::readObject(struct pickle * pickle, int fd) {
  DtsObject datump;
  struct sockdatum  hdr;
  int temp = receive_it(fd, (unsigned char*)&hdr, sizeof(struct sockdatum));

  //fprintf(stderr, "recvd datum of type %d, size %d\n", hdr.datum->gettype(), hdr.datum->getsize());

  if (temp <= 0) return NULL;
  
  if (hdr.namelen)  {
    if ((int)pickle_getnewtype(fd, &hdr, pickle) < 0) 
      return NULL;
  }

  assert(hdr.datum->getsize() + sizeof(struct sockdatum) > 0);
  datump = newObject(maptype(pickle, hdr.datum->gettype(), fd),
		     hdr.datum->getsize() + sizeof(DtsObject));
  
  if ((temp = receive_it(fd, datump->getdata(), hdr.datum->getsize())) < 0) {
    
    return NULL;
  }

  return datump;
}

/* 0 is EOF, -1 is error midstream */
int DtsObject_::write(struct pickle * pickle, int fd) {
  struct sockdatum hdr;
  int * typep = get_type(pickle, fd, this->gettype());
  char * name = NULL;
    
  memcpy(&hdr.datum, this, sizeof(DtsObject));
  hdr.namelen = 0;

  if (! *typep) {
    *typep = 1;

    name = dts->typename_bynum(hdr.datum->gettype());
    hdr.namelen=strlen(name);
    //fprintf(stderr, "Sending new type %d -> %s\n", &hdr.datum->gettype(), name);
  } else {
    //name=dts->typename_bynum(&hdr.datum->gettype());
    //fprintf(stderr, "Sending old type %d -> %s\n", &hdr.datum->gettype(), name);
  }

  if (1 > send_it(fd, (unsigned char*)&hdr, sizeof(struct sockdatum)))  
    return -1;
  if (hdr.namelen && (1 > send_it(fd, (unsigned char*)name, hdr.namelen)))
    return -1;
  if (1 > send_it(fd, hdr.datum->getdata(), hdr.datum->getsize()))  
    return -1;

  return 1;
}
