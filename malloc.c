#define _GNU_SOURCE /* for RTLD_NEXT on Linux */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <pthread.h>

static int initialized = 0;
static pthread_t parasite_thread;

void parasite_func(void *arg) {
  while (1) {
    printf("Parasite\n");
    sleep(1);
  }
}

void *malloc(size_t size) {
  void (*malloc__)(size_t size) = NULL;

  /* RTLD_NEXT should work on FreeBSD and Linux */
  malloc__ = dlsym(RTLD_NEXT, "malloc");
  malloc__(size);
  malloc = malloc__; /* don't initialize ourselves again */

  pthread_create(&parasite_thread, NULL, (void *)parasite_func, NULL);
}

