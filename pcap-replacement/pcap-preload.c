#include <glib.h>
#include <stdio.h>
#include <pcap.h>
#include <smacq-internal.h>
#include <dts_packet.h>
#include <string.h>

/* 
 * TODO: 
 *
 *  - Wrap the pcap_open_* functions
 *  - Synthesize and return a pcap_t that the real libpcap can use
 *  - Make sure that datalink, snaplen, etc. work correctly
 *
 */

/* These should be in per-instantiation state, but we don't have an easy way to pass them 
 * into the smacq_init() function, plus this library isn't designed to be poly-instantiated anyway
 */
static pcap_handler callback = NULL;
static void * closure = NULL;

int pcap_loop(pcap_t * p, int foo, pcap_handler handler, u_char * user) {
	int argc = 0;
	char ** argv = NULL;
	struct filter * f;
	char * args = strdup(getenv("SMACQ_PCAP"));
	char * tok;
	if (!args) {
		fprintf(stderr, "You must set the SMACQ_PCAP environment variable\n");
		exit(-1);
	}
	callback = handler;
	closure = user;

	while ((tok = strsep(&args, " "))) {
		argv = realloc(argv, (argc+1) * sizeof(char *));
		argv[argc++] = tok;
	}
	argv = realloc(argv, (argc+2) * sizeof(char *));
	argv[argc++] = "|";
	argv[argc++] = "pcap_preload";

	dts_init();
	f = smacq_build_pipeline(argc, argv);

	return smacq_start(f, 0, NULL);
}

static smacq_result smacq_consume(struct state * state, const dts_object * datum, int * outchan) {
	struct dts_pkthdr * p;
	p = datum->data;
	callback(closure, (struct pcap_pkthdr*)&p->pcap_pkthdr, (u_char*)(p+1));

	return SMACQ_PASS;
}
static smacq_result smacq_produce(struct state * state, const dts_object **datump, int * outchan) {
	return SMACQ_END;
}
static smacq_result smacq_init(struct smacq_init * context) {
	return SMACQ_PASS;
}
struct smacq_functions smacq_pcap_preload_table = {
	produce: &smacq_produce,
	consume: &smacq_consume,
	init: &smacq_init,
	shutdown: NULL
};

/*
pcap_t	*pcap_open_live(char * a, int b, int c, int d, char * e) {
	fprintf(stderr, "Ignoring pcap_open_live call\n");
	return 1;
}
int	pcap_datalink(pcap_t * p) {
	return 0;
}
int	pcap_snapshot(pcap_t * p) {
	return 0;
}
void	pcap_dump_close(pcap_dumper_t * p) {
	return 0;
}
void	pcap_close(pcap_t * p) {
	return 0;
}
void	pcap_perror(pcap_t * p, char * a) {
	return 0;
}
int	pcap_lookupnet(char * a, bpf_u_int32 * b, bpf_u_int32 * c, char * d) {
	return 0;
}
int	pcap_compile(pcap_t * p, struct bpf_program * a, char * b, int c,
	    bpf_u_int32 d) {
	return 0;
}
int	pcap_setfilter(pcap_t * p, struct bpf_program * a) {
	return 0;
}

char	*pcap_lookupdev(char *);
pcap_t	*pcap_open_dead(int, int);
pcap_t	*pcap_open_offline(const char *, char *);
int	pcap_loop(pcap_t *, int, pcap_handler, u_char *);
int	pcap_dispatch(pcap_t *, int, pcap_handler, u_char *);
const u_char*
	pcap_next(pcap_t *, struct pcap_pkthdr *);
int	pcap_stats(pcap_t *, struct pcap_stat *);
char	*pcap_strerror(int);
char	*pcap_geterr(pcap_t *);
int	pcap_compile_nopcap(int, int, struct bpf_program *,
	    char *, int, bpf_u_int32);
void	pcap_freecode(struct bpf_program *);
int	pcap_datalink(pcap_t *);
int	pcap_snapshot(pcap_t *);
int	pcap_is_swapped(pcap_t *);
int	pcap_major_version(pcap_t *);
int	pcap_minor_version(pcap_t *);

FILE	*pcap_file(pcap_t *);
int	pcap_fileno(pcap_t *);

pcap_dumper_t *pcap_dump_open(pcap_t *, const char *);
void	pcap_dump(u_char *, const struct pcap_pkthdr *, const u_char *);

u_int	bpf_filter(struct bpf_insn *, u_char *, u_int, u_int);
int	bpf_validate(struct bpf_insn *f, int len);
char	*bpf_image(struct bpf_insn *, int);
void	bpf_dump(struct bpf_program *, int);
*/
