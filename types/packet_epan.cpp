#ifdef WITH_ETHEREAL
#include <dts_packet.h>
#include <dts.h>
#include <string>

extern "C" {
#include <epan/../config.h>
#include <epan/epan_dissect.h>
#include <epan/ftypes/ftypes.h>
#include <epan/packet.h>
#include <epan/timestamp.h>
#include <register.h>
#include <epan/prefs.h>
}

ts_type timestamp_type = TS_ABSOLUTE;

static int did_epan_init = 0;

static void open_failure_message(const char *filename, int err, gboolean for_writing)
{
  fprintf(stderr, "tethereal: ");
  //fprintf(stderr, file_open_error_message(err, for_writing), filename);
  fprintf(stderr, "\n");
}

/*
 * General errors are reported with an console message in Tethereal.
 */
static void
failure_message(const char *msg_format, va_list ap)
{
  fprintf(stderr, "tethereal: ");
  vfprintf(stderr, msg_format, ap);
  fprintf(stderr, "\n");
}

/*
 * Read errors are reported with an console message in Tethereal.
 */
static void
read_failure_message(const char *filename, int err)
{
  fprintf(stderr, "tethereal: An error occurred while reading from the file \"%s\": %s.\n",
          filename, strerror(err));
}

int epan_getname(struct dts_pkthdr * dhdr, const char * dts_name, DtsObject fieldo, DTS * dts) {
  int len;
  int field_id;
  field_info * finfo;
  GPtrArray	*finfos;
  header_field_info * hfinfo;
  epan_dissect_t * edt;
  frame_data fdata;
  
  struct old_pcap_pkthdr * phdr = &dhdr->pcap_pkthdr;
  guint8 * packet_data = (guint8*)(dhdr+1);
  
  memset(&fdata, 0, sizeof(frame_data));
  
  fdata.pkt_len = phdr->len;
  fdata.cap_len = phdr->caplen;
  fdata.lnk_t = 0; /* getfield */
  
  if (!did_epan_init) {
    epan_init(PLUGIN_DIR,register_all_protocols,register_all_protocol_handoffs,
            failure_message,open_failure_message,read_failure_message);

    //epan_init(".", register_all_protocols, register_all_protocol_handoffs);
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
  
  finfo = (field_info*)g_ptr_array_index(finfos, 0);
  assert(finfo);
  
  
  switch(hfinfo->type) {
  case FT_STRING:
    fieldo->settype(dts->requiretype("string"));
    fieldo->setsize(fvalue_length(&finfo->value));
    fieldo->setdata(fvalue_get(&finfo->value));
    break;
    
  case FT_DOUBLE:
    fieldo->settype(dts->requiretype("double"));
    dts_data_as(fieldo, double) = fvalue_get_floating(&finfo->value);
    break;
    
  case FT_NONE:
    fieldo->settype(dts->requiretype("empty"));
    break;
    
  case FT_BOOLEAN:
  case FT_UINT8:
    fieldo->settype(dts->requiretype("ubyte"));
    dts_data_as(fieldo, unsigned char) = fvalue_get_integer(&finfo->value);
    break;
    
  case FT_PROTOCOL:
    fieldo->settype(dts->requiretype("ubyte"));
    dts_data_as(fieldo, unsigned char) = fvalue_get_integer(&finfo->value);
    break;
    
  case FT_ETHER:
    fieldo->settype(dts->requiretype("macaddr"));
    fieldo->setdata(fvalue_get(&finfo->value));
    break;
    
  case FT_UINT16:
    fieldo->settype(dts->requiretype("ushort"));
    dts_data_as(fieldo, uint16_t) = fvalue_get_integer(&finfo->value);
    break;
    
  case FT_UINT32:
    fieldo->settype(dts->requiretype("uint32"));
    dts_data_as(fieldo, uint32_t) = fvalue_get_integer(&finfo->value);
    break;
    
  case FT_IPv4:
    fieldo->settype(dts->requiretype("ip"));
    fieldo->setdata(fvalue_get(&finfo->value));
    dts_data_as(fieldo, unsigned long) = htonl(dts_data_as(fieldo, unsigned long));
    break;
    
  default:
    fprintf(stderr, "packet: warning: unsupported EPAN type %s being converted to 'string'\n", ftype_pretty_name(hfinfo->type));
    fieldo->settype(dts->requiretype("string"));
    fieldo->setdata(fvalue_get(&finfo->value));
    fieldo->setsize(fvalue_length(&finfo->value));
    break;
  }
  
  epan_dissect_free(edt);
  //free_data_sources(&fdata);
  
  return 1;
  
  //proto_tree_free_node();
}

int epan_getfield(DtsObject packet, DtsObject fieldo, dts_field_element element) {
  char * dts_name = strdup(packet->getDts()->field_getname(element));
  char * p = dts_name;

  while (p && (p = index(p, ':'))) {
    p[0] = '.';
  }
  int retval = epan_getname((struct dts_pkthdr*)packet->getdata(), dts_name, fieldo, packet->getDts());

  return retval;
}


#endif /* WITH_ETHEREAL */
