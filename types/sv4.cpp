#include <sys/types.h>
#include <netinet/in.h>
#include <dts-module.h>

static int get_sv4_data(DtsObject datum, DtsObject data) {
	data->setdata(datum->getdata() + 51);
	data->setsize(datum->getsize() - 51);
	return (data->getsize() > 0);
}
static int get_sv4_prompt(DtsObject o, DtsObject stro) {
	void * p = o->getdata()+ 47;
	ushort prompt = ntohs( *(ushort*)p );
	stro->setsize(6);
	unsigned char * str = stro->getdata();

	str[0] = ((prompt & 16) ? '<' : ' ');
	str[1] = ((prompt & 8) ? '<' : ' ');
	str[2] = ((prompt & 4) ? '*' : ' ');
	str[3] = ((prompt & 2) ? '>' : ' ');
	str[4] = ((prompt & 1) ? '>' : ' ');
	str[5] = '\0';
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
	{ "ntime",	"start",	NULL },
	{ "nushort",	"startms",	NULL },
	{ "ntime",	"stop",		NULL },
	{ "nushort",	"stopms",	NULL },
	{ "ubyte",	"ipprotocol",	NULL },
	{ "nushort",	"prompt",	NULL },
	{ "string",	"promptstr",	get_sv4_prompt },
	{ "bytes",	"payload",	get_sv4_data },
        { NULL,		NULL,		NULL }
};

struct dts_type_info dts_type_sv4_table = {
	size: -1,
};
