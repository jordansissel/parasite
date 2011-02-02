#define _GNU_SOURCE /* for RTLD_NEXT on Linux */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <pthread.h>
#include <string.h>

static int initialized = 0;
static pthread_t parasite_thread;
static void (*malloc__)(size_t size) = NULL;
static int (*open__)(const char *, int, ...) = NULL;

void parasite_func(void *arg) {
  while (1) {
    printf("Parasite\n");
    sleep(1);
  }
}

void parasite_init(const char *origin_func) {
  if (initialized) {
    return;
  }

  printf("Parasite initialized by hooking '%s'\n", origin_func);
  initialized = 1;
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
