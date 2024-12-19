#ifndef CAT
#define CAT

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct flags_alg {
  int n, b, s, s1, text;
} flags_alg;

typedef struct flags {
  int b, e, v, n, s, t, line, none, files;
  flags_alg temp_f;
} flags;

#endif