#include <sys/types.h>
#include <netinet/in.h>
#include <dts-module.h>

static int get_sv4_start(DtsObject o, DtsObject data) {
	struct timeval * tv = (struct timeval*)data->getdata();
	tv->tv_sec = ntohl(*(time_t*)(o->getdata() + 34));
	tv->tv_usec = 1000 * ntohs(*(ushort*)(o->getdata() + 38));
	return 1;
}

static int get_sv4_stop(DtsObject o, DtsObject data) {
	struct timeval * tv = (struct timeval*)data->getdata();
	tv->tv_sec = ntohl(*(time_t*)(o->getdata() + 40));
	tv->tv_usec = 1000 * ntohs(*(ushort*)(o->getdata() + 44));
	return 1;
}

static int get_sv4_data(DtsObject datum, DtsObject data) {
	data->setdata(datum->getdata() + 51);
	data->setsize(datum->getsize() - 51);
	return (data->getsize() > 0);
}

static int get_sv4_prompt(DtsObject o, DtsObject stro) {
	void * p = o->getdata()+ 47;
	ushort prompt = ntohs( *(ushort*)p );
	stro->setsize(5);
	unsigned char * str = stro->getdata();

	str[0] = ((prompt & 16) ? '<' : ' ');
	str[1] = ((prompt & 8) ? '<' : ' ');
	str[2] = ((prompt & 4) ? '*' : ' ');
	str[3] = ((prompt & 2) ? '>' : ' ');
	str[4] = ((prompt & 1) ? '>' : ' ');
	/* sprintf(str+5, " %d", prompt); */

	return 1;
}

struct dts_field_spec dts_type_sv4_fields[] = {
	{ "nushort",	"epoch",	NULL },
	{ "nuint32",	"id",		NULL },
	{ "ip",		"dstip",	NULL },
	{ "ip",		"srcip",	NULL },
	{ "nushort",	"dstport",	NULL },
	{ "nushort",	"srcport",	NULL },
	{ "nuint32",	"dstpackets",	NULL },
	{ "nuint32",	"srcpackets",	NULL },
	{ "nuint32",	"dstbytes",	NULL },
	{ "nuint32",	"srcbytes",	NULL },
	{ "ntime",	"starts",	NULL },
	{ "nushort",	"startms",	NULL },
	{ "ntime",	"stops",	NULL },
	{ "nushort",	"stopms",	NULL },
	{ "ubyte",	"ipprotocol",	NULL },
	{ "nushort",	"prompt",	NULL },
	{ "string",	"promptstr",	get_sv4_prompt },
	{ "string",	"payload",	get_sv4_data },
	{ "timeval",	"start",	get_sv4_start },
	{ "timeval",	"stop",		get_sv4_stop },
        { NULL,		NULL,		NULL }
};

struct dts_type_info dts_type_sv4_table = {
	size: -1,
};
