#include <smacq.h>

static int get_sv4_data(const dts_object * datum, dts_object * data) {
	data->data = datum->data + 51;
	data->len = datum->len - 51;
	return (data->len > 0);
}

struct dts_field_spec dts_type_sv4_fields[] = {
	{ "uint64",	"id",		NULL },
	{ "ip",		"dstip",	NULL },
	{ "ip",		"srcip",	NULL },
	{ "ushort",	"dstport",	NULL },
	{ "ushort",	"srcport",	NULL },
	{ "uint32",	"dstpackets",	NULL },
	{ "uint32",	"srcpackets",	NULL },
	{ "uint32",	"dstbytes",	NULL },
	{ "uint32",	"srcbytes",	NULL },

	{ "ip",		"nexthop",	NULL },
	{ "time",	"start",	NULL },
	{ "ushort",	"startms",	NULL },
	{ "time",	"stop",		NULL },
	{ "ushort",	"stopms",	NULL },
	{ "ubyte",	"ipprotocol",	NULL },
	{ "ushort",	"prompt",	NULL },
	{ "bytes",	"payload",	get_sv4_data },
        { END,		NULL,		NULL }
};

struct dts_type_info dts_type_sv4_table = {
	size: -1,
};
