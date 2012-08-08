#include <dts-module.h>

struct dts_field_spec dts_type_netflow7_fields[] = {
	{ "timeval",	"end",		NULL }, /*  uint32_t unix_secs; uint32_t unix_nsecs;      * Current seconds & ns since 0000 UTC 1970 */
	{ "uint32",	"sysUpTime",	NULL }, /*   uint32_t sysUpTime;      * Current time in millisecs since router booted */
	{ "ip",		"router",	NULL }, /*   uint32_t exaddr;         * Exporter IP address */
	{ "ip",		"srcip",	NULL }, /*   uint32_t srcaddr;        * Source IP Address */
	{ "ip",		"dstip",	NULL }, /*   uint32_t dstaddr;        * Destination IP Address */
	{ "ip",		"nexthop",	NULL }, /*   uint32_t nexthop;        * Next hop router's IP Address */
	{ "ushort",	"input",	NULL }, /*   uint16_t input;          * Input interface index */
	{ "ushort",	"output",	NULL }, /*   uint16_t output;         * Output interface index */
	{ "uint32",	"packets",	NULL }, /*   uint32_t dPkts;          * Packets sent in Duration */
	{ "uint32",	"bytes",	NULL }, /*   uint32_t dOctets;        * Octets sent in Duration. */
	{ "timeval",	"start",	NULL }, /*   uint32_t First;          * unix_secs at start of flow */
	{ "ushort",	"srcport",	NULL }, /*   uint16_t srcport;        * TCP/UDP source port number or equivalent */
	{ "ushort",	"dstport",	NULL }, /*   uint16_t dstport;        * TCP/UDP destination port number or equiv */
	{ "ubyte",	"ipprotocol",	NULL }, /*   uint8_t  prot;           * IP protocol, e.g., 6=TCP, 17=UDP, ... */
	{ "ubyte",	"tos",		NULL }, /*   uint8_t  tos;            * IP Type-of-Service */
	{ "ubyte",	"tcp_flags",	NULL }, /*   uint8_t  tcp_flags;      * OR of TCP header bits */
	{ "ubyte",	"flags",	NULL }, /*   uint8_t  flags;          * Reason flow discarded, etc */
	{ "ubyte",	"engine_type",	NULL }, /*   uint8_t  engine_type;    * Type of flow switching engine (RP,VIP,etc.) */
	{ "ubyte",	"engine_id",	NULL }, /*   uint8_t  engine_id;      * Slot number of the flow switching engine */
	{ "ubyte",	"src_mask",	NULL }, /*   uint8_t  src_mask;       * mask length of source address */
	{ "ubyte",	"dst_mask",	NULL }, /*   uint8_t  dst_mask;       * mask length of destination address */
	{ "ushort",	"srcAS",	NULL }, /*   uint16_t src_as;         * AS of source address */
	{ "ushort",	"dstAS",	NULL }, /*   uint16_t dst_as;         * AS of destination address */
	{ "uint32",	"router_sc",	NULL }, /*   uint32_t router_sc;      * ID of router shortcut by switch */
        { NULL,		NULL,		NULL }
};

struct dts_type_info dts_type_netflow7_table = {
	size: 68,
};
