#include <dts-module.h>

struct dts_field_spec dts_type_cflow_fields[] = {
	{ "nuint32",	"index",	NULL },
	{ "ip",		"router",	NULL },
	{ "ip",		"srcip",	NULL },
	{ "ip",		"dstip",	NULL },
	{ "nushort",	"input",	NULL },
	{ "nushort",	"output",	NULL },
	{ "nushort",	"srcport",	NULL },
	{ "nushort",	"dstport",	NULL },
	{ "nuint32",	"packets",	NULL },
	{ "nuint32",	"bytes",	NULL },
	{ "ip",		"nexthop",	NULL },
	{ "ntime",	"start",	NULL },
	{ "ntime",	"end",		NULL },
	{ "ubyte",	"ipprotocol",	NULL },
	{ "ubyte",	"tos",		NULL },
	{ "nushort",	"srcAS",	NULL },
	{ "nushort",	"dstAS",	NULL },
	{ "ubyte",	"src_mask",	NULL },
	{ "ubyte",	"dst_mask",	NULL },
	{ "ubyte",	"tcp_flags",	NULL },
	{ "ubyte",	"enginetype",	NULL },
	{ "ubyte",	"engineid",	NULL },
        { NULL,		NULL,		NULL }
};

struct dts_type_info dts_type_cflow_table = {
	size: 55,
};
