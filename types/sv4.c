#include <smacq.h>

static int get_sv4_data(const dts_object * datum, dts_object * data) {
	return 0;
}

struct dts_field_spec dts_type_sv4_fields[] = {
	{ "uint64",	"id",		NULL },
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
	{ "bytes",	"data",		get_sv4_data },
        { END,		NULL,		NULL }
};

struct dts_type_info dts_type_sv4_table = {
	size: -1,
};
