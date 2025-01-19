#ifndef GREP
#define GREP

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 4096

typedef struct Flags {
  int e;

  int i;
  int v;
  int c;
  int n;
  int l;
  int h;
  int s;
  int f;
  int o;
  int err;

  int regex_flags;
  int many_files;
} Flags;

typedef struct pos {
  int shablon_len;
  char *shablons;

  int length_args;
  char **args;
} pos;

void run_grep(pos *pos, Flags *greps_f);
void match_files(pos *pos, regex_t *regex, Flags *greps_f);
void apply_shablon(char *file_name, regex_t *regex, Flags *greps_f);
void check_flags(Flags *greps_f, pos *pos);
void position_arg(pos *pos, Flags *greps_f, int argc, char *argv[]);
void get_pos_args(pos *pos, Flags *greps_f, int argc, char *argv[]);
void get_shablon_from_file(pos *pos, char *file_name, Flags *greps_f);
void handle_flags(pos *pos, Flags *greps_f, int argc, char *argv[]);
void delete_last_pipe(char *shablon);
void print_error();
void init_flags(Flags *flag);
void init_shablons(pos *position, int argc);
void free_pos(pos *position);
void shablon_cat(pos *pos, char *src);
int find_regex(regex_t *regex, char *line, regmatch_t **matches);
void print_prefix(Flags *greps_f, char *file_path, int current_line_number);
void print_plain_result(char *line, Flags *greps_f, char *file_path,
                        int current_line_number);
void print_result_flag_o(char *line, Flags *greps_f, char *file_path,
                         int current_line_number, regmatch_t *matches,
                         int matches_count);

#endif