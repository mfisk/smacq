#include <smacq.h>
#include <smacq_am.h>
#include <upcr.h>

void nothing() {
	return;
}

extern "C" {
extern void upcri_init_heaps(void *start, uintptr_t len);
extern void upcri_init_cache(void *start, uintptr_t len);


upcr_thread_t  	UPCRL_static_thread_count       = 0;
uintptr_t       UPCRL_default_shared_size       = 64*1024;
uintptr_t       UPCRL_default_shared_offset     = 0;
int		UPCRL_progress_thread		= 0;	
uintptr_t       UPCRL_default_cache_size        = 0;
int             UPCRL_attach_flags              = 0;
upcr_thread_t	UPCRL_default_pthreads_per_node	= 4;
const char *    UPCRL_main_name		= "main";
void           (*UPCRL_pre_spawn_init)() = NULL;
void           (*UPCRL_per_pthread_init)() = NULL;
void (*UPCRL_heap_init)(void * start, uintptr_t len) = upcri_init_heaps;
void (*UPCRL_static_init)(void *start, uintptr_t len) = nothing;
void (*UPCRL_cache_init)(void *start, uintptr_t len) = upcri_init_cache;
void (*UPCRL_mpi_init)(int *pargc, char ***pargv) = NULL;
void (*UPCRL_mpi_finalize)() = NULL;
void (*UPCRL_profile_finalize)() = NULL;

 int main(int argc, char ** argv) {
  bupc_init(&argc, &argv);
 // upcr_startup_init(&argc, &argv, 0, 0, "main");
  //upcr_startup_attach(64*1024, 0, 0);

  //struct upcr_startup_spawnfuncs spawnfuncs = {NULL, NULL, NULL, upcri_init_heaps, nothing, NULL};
  //upcr_startup_spawn(&argc, &argv, 0, 0, &spawnfuncs);

  if (gasnet_mynode() == 0) {
	smacqq(argc, argv);
	bupc_exit(0);
	return 0;
  } else {
	fprintf(stderr, "Slave!\n");
	smacq_am_slave_loop();
	return 0;
  }
 }
}

