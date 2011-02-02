#define _GNU_SOURCE /* for RTLD_NEXT on Linux */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <pthread.h>
#include <string.h>

/* Add a ruby interpreter */
#include "ruby.h"

static int initialized = 0;
static pthread_t parasite_thread;
static void (*malloc__)(size_t size) = NULL;
static int (*open__)(const char *, int, ...) = NULL;

struct parasite {
  int pid;
};

static struct parasite parasite;

void parasite_func(void *arg) {
  //system("ulimit -a");
  //system("id");
  //while (1) {
    //printf("Parasite[%d]\n", parasite.pid);
    //sleep(1);
  //}
  
  ruby_init();
  ruby_init_loadpath();
  rb_load_file(getenv("PARASITE_RUBY"));
  ruby_exec();
  ruby_finalize();

  printf("parasite ruby ended\n");
}

void parasite_init(const char *origin_func) {
  if (initialized) {
    return;
  }

  initialized = 1;
  printf("Parasite initialized by hooking '%s'\n", origin_func);
  parasite.pid = getpid();
  pthread_create(&parasite_thread, NULL, (void *)parasite_func, NULL);
}

void *malloc(size_t size) {
  if (!malloc__) {
    /* RTLD_NEXT should work on FreeBSD and Linux */
    malloc__ = dlsym(RTLD_NEXT, "malloc");
    parasite_init("malloc");
  }
  malloc__(size);
}

int open(const char *path, int mode, ...) {
  if (!open__) {
    /* RTLD_NEXT should work on FreeBSD and Linux */
    open__ = dlsym(RTLD_NEXT, "open");
    parasite_init("open");
  }
  return open__(path, mode);
}
