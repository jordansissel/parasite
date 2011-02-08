#define _GNU_SOURCE /* for RTLD_NEXT on Linux */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <pthread.h>
#include <string.h>

/* For bind(2) */
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>


/* Add a ruby interpreter */
#include "ruby.h"

static int initialized = 0;
static pthread_t parasite_thread;
static void* (*malloc__)(size_t size) = NULL;
static int (*open__)(const char *, int) = NULL;
static int (*bind__)(int sockfd, const struct sockaddr *addr,
                     socklen_t addrlen) = NULL;

struct parasite {
  int pid;
};

static struct parasite parasite;

void parasite_func(void *arg) {
  ruby_init();
  ruby_init_loadpath();
  const char *script = getenv("PARASITE_RUBY");
  if (script == NULL) {
    fprintf(stderr, "No script given. Missing PARASITE_RUBY in environment?\n");
    return;
  }
  rb_load_file(script);
  ruby_exec();
  ruby_finalize();

  fprintf(stderr, "parasite ruby ended\n");
}

void parasite_init(const char *origin_func) {
  if (initialized) {
    return;
  }

  initialized = 1;
  fprintf(stderr, "Parasite initialized by hooking '%s'\n", origin_func);
  parasite.pid = getpid();
  pthread_create(&parasite_thread, NULL, (void *)parasite_func, NULL);
}

void *malloc(size_t size) {
  if (!malloc__) {
    /* RTLD_NEXT should work on FreeBSD and Linux */
    malloc__ = dlsym(RTLD_NEXT, "malloc");
    parasite_init("malloc");
  }
  return malloc__(size);
}

int open(const char *path, int mode, ...) {
  if (!open__) {
    /* RTLD_NEXT should work on FreeBSD and Linux */
    open__ = dlsym(RTLD_NEXT, "open");
    parasite_init("open");
  }
  return open__(path, mode);
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
  if (!bind__) {
    /* RTLD_NEXT should work on FreeBSD and Linux */
    bind__ = dlsym(RTLD_NEXT, "bind");
    parasite_init("bind");
  }

  return bind__(sockfd, addr, addrlen);
}

