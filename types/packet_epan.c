#ifdef WITH_ETHEREAL
#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <limits.h>

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <glib.h>

#include <epan/epan.h>
#include <epan/epan_dissect.h>
#include <epan/ftypes/ftypes.h>
#include <epan/proto.h>
#include <epan/packet.h>
#include <epan/timestamp.h>
#include <register.h>
#include <prefs.h>

ts_type timestamp_type = TS_ABSOLUTE;

static int did_epan_init = 0;

int epan_getname(struct dts_pkthdr * dhdr, char * dts_name) {
  int len;
  int field_id;
  field_info * finfo;
  GPtrArray	*finfos;
  header_field_info * hfinfo;
  epan_dissect_t * edt;
  frame_data fdata;
  
  struct old_pcap_pkthdr * phdr = &dhdr->pcap_pkthdr;
  char * packet_data = (char*)(dhdr+1);
  
  memset(&fdata, 0, sizeof(frame_data));
  
  fdata.pkt_len = phdr->len;
  fdata.cap_len = phdr->caplen;
  fdata.lnk_t = 0; /* getfield */
  
  if (!did_epan_init) {
    epan_init(".", register_all_protocols, register_all_protocol_handoffs);
    prefs_register_modules();
    init_dissection();
    did_epan_init =1;
  }
  
  //fprintf(stderr, "Getting proto %s\n", dts_name);
  
  hfinfo =  proto_registrar_get_byname(dts_name);
  
  if (!hfinfo) {
    fprintf(stderr, "EPAN does not recognize field %s\n", dts_name);
    return 0;
  }
  field_id = hfinfo->id;

  edt = epan_dissect_new(TRUE, TRUE);
  proto_tree_prime_hfid(edt->tree, field_id);
  epan_dissect_run(edt, NULL, packet_data, &fdata, NULL);
  
  /* The rest of this could be a subfield of an "epan" type */
  
  finfos = proto_get_finfo_ptr_array(edt->tree, field_id);
  if (!finfos) {
    fprintf(stderr, "Unexpected error: EPAN field not requested\n");
    return 0;
  }
  
  len = finfos->len;
  
  if (len == 0) {
    //fprintf(stderr, "packet: EPAN field not present\n");
    return 0;
  }
  
  if (len > 1) {
    fprintf(stderr, "packet: warning: only returning first of multiple values\n");
  }
  
  finfo = g_ptr_array_index(finfos, 0);
  assert(finfo);
  
  
  switch(hfinfo->type) {
  case FT_STRING:
    fieldo->setype(packet->tenv->requiretype("string"));
    fieldo->setsize(fvalue_length(finfo->value));
    fieldo->setdata(fvalue_get(finfo->value));
    break;
    
  case FT_DOUBLE:
    fieldo->settype(packet->tenv->requiretype("double"));
    dts_data_as(fieldo, double) = fvalue_get_floating(finfo->value);
    break;
    
  case FT_NONE:
    fieldo->settype = packet->tenv->requiretype("empty");
    break;
    
  case FT_BOOLEAN:
  case FT_UINT8:
    fieldo->settype = packet->tenv->requiretype("ubyte");
    dts_data_as(fieldo, unsigned char) = fvalue_get_integer(finfo->value);
    break;
    
  case FT_PROTOCOL:
    fieldo->settype = packet->tenv->requiretype("ubyte");
    dts_data_as(fieldo, unsigned char) = fvalue_get_integer(finfo->value);
    break;
    
  case FT_ETHER:
    fieldo->settype = packet->tenv->requiretype("macaddr");
    fieldo->getdata() = fvalue_get(finfo->value);
    break;
    
  case FT_UINT16:
    fieldo->type = packet->tenv->requiretype("ushort");
    dts_data_as(fieldo, uint16_t) = fvalue_get_integer(finfo->value);
    break;
    
  case FT_UINT32:
    fieldo->type = packet->tenv->requiretype("uint32");
    dts_data_as(fieldo, uint32_t) = fvalue_get_integer(finfo->value);
    break;
    
  case FT_IPv4:
    fieldo->type = packet->tenv->requiretype("ip");
    fieldo->getdata() = fvalue_get(finfo->value);
    dts_data_as(fieldo, unsigned long) = htonl(dts_data_as(fieldo, unsigned long));
    break;
    
  default:
    fprintf(stderr, "packet: warning: unsupported EPAN type %s being converted to 'bytes'\n", ftype_pretty_name(hfinfo->type));
    fieldo->settype(packet->tenv->requiretype("bytes"));
    fieldo->setdata(fvalue_get(finfo->value));
    fieldo->setsize(fvalue_length(finfo->value));
    break;
  }
  
  epan_dissect_free(edt);
  //free_data_sources(&fdata);
  
  return 1;
  
  //proto_tree_free_node();
}
#endif /* WITH_ETHEREAL */
