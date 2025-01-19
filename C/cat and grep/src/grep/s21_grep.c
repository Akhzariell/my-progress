#include "s21_grep.h"

int main(int argc, char *argv[]) {
  if (argc >= 3) {
    Flags greps_f;
    pos position;

    init_flags(&greps_f);
    init_shablons(&position, argc);

    position_arg(&position, &greps_f, argc, argv);

    if (!greps_f.err) {
      run_grep(&position, &greps_f);
    } else {
      print_error();
    }
    free_pos(&position);
  } else {
    print_error();
  }

  return 0;
}

void run_grep(pos *pos, Flags *greps_f) {
  regex_t regex;
  int errcode;

  if ((errcode = regcomp(&regex, pos->shablons, greps_f->regex_flags)) == 0) {
    match_files(pos, &regex, greps_f);
  } else if (!greps_f->s) {
    char BUFF[256];
    regerror(errcode, &regex, BUFF, 256);
    perror(BUFF);
  }
  regfree(&regex);
}

void match_files(pos *pos, regex_t *regex, Flags *greps_f) {
  for (int i = 0; i < pos->length_args; i++) {
    apply_shablon(pos->args[i], regex, greps_f);
  }
}

void apply_shablon(char *file_name, regex_t *regex, Flags *greps_f) {
  FILE *file = fopen(file_name, "r");

  if (file == NULL) {
    if (!greps_f->s) perror("No such file or directory");
  } else {
    char buff_line[SIZE] = {0};
    int match_file = 0;
    int current_line_number = 1;
    int count_valid_lines = 0;

    while (fgets(buff_line, SIZE - 1, file) != NULL) {
      regmatch_t *matches;
      int matches_count = find_regex(regex, buff_line, &matches);
      int match = matches_count != 0;

      if (greps_f->v) {
        match = !match;
      }

      if (match) {
        match_file = 1;
        if (greps_f->l) {
          free(matches);
          break;
        }

        if (!greps_f->c) {
          if (!greps_f->o) {
            print_plain_result(buff_line, greps_f, file_name,
                               current_line_number);
          } else if (!greps_f->v) {
            print_result_flag_o(buff_line, greps_f, file_name,
                                current_line_number, matches, matches_count);
          }
        }

        ++count_valid_lines;
      }

      ++current_line_number;
      free(matches);
    }

    if (match_file) {
      if (greps_f->l) {
        printf("%s\n", file_name);
      } else if (greps_f->c) {
        if (greps_f->many_files && !greps_f->h) {
          printf("%s:", file_name);
        }

        printf("%d\n", count_valid_lines);
      }
    }

    fclose(file);
  }
}

int find_regex(regex_t *regex, char *line, regmatch_t **matches) {
  *matches = calloc(1, sizeof(regmatch_t));

  int matches_count = 0;
  int offset = 0;

  while (1) {
    int result = regexec(regex, line + offset, 1, *matches + matches_count, 0);

    (*matches)[matches_count].rm_so += offset;
    (*matches)[matches_count].rm_eo += offset;

    offset = (int)(*matches)[matches_count].rm_eo;

    if (result == 0) {
      ++matches_count;
      *matches = realloc(*matches, (matches_count + 1) * sizeof(regmatch_t));
    } else {
      break;
    }
  }

  return matches_count;
}

void print_plain_result(char *line, Flags *greps_f, char *file_path,
                        int current_line_number) {
  print_prefix(greps_f, file_path, current_line_number);

  printf("%s", line);

  if (strchr(line, '\n') == NULL) {
    printf("\n");
  }
}

void print_result_flag_o(char *line, Flags *greps_f, char *file_path,
                         int current_line_number, regmatch_t *matches,
                         int matches_count) {
  for (int i = 0; i < matches_count; ++i) {
    print_prefix(greps_f, file_path, current_line_number);
    printf("%.*s\n", (int)(matches[i].rm_eo - matches[i].rm_so),
           line + matches[i].rm_so);
  }
}

void print_prefix(Flags *greps_f, char *file_path, int current_line_number) {
  if (greps_f->many_files && !greps_f->h) {
    printf("%s:", file_path);
  }

  if (greps_f->n) {
    printf("%d:", current_line_number);
  }
}

void check_flags(Flags *greps_f, pos *pos) {
  greps_f->regex_flags = REG_EXTENDED;

  if (greps_f->i) {
    greps_f->regex_flags |= REG_ICASE;
  }

  greps_f->many_files = pos->length_args > 1;
}

void position_arg(pos *pos, Flags *greps_f, int argc, char *argv[]) {
  handle_flags(pos, greps_f, argc, argv);

  if (!greps_f->err) {
    get_pos_args(pos, greps_f, argc, argv);

    check_flags(greps_f, pos);
  }
}

void get_pos_args(pos *pos, Flags *greps_f, int argc, char *argv[]) {
  int count = 0, flag = 0;

  for (int i = 1; i < argc; ++i) {
    int len = strlen(argv[i]) - 1;

    if (argv[i][0] != '-' && !flag) {
      pos->args[count] = argv[i];
      ++count;
    }

    if (argv[i][0] == '-' && (argv[i][len] == 'e' || argv[i][len] == 'f')) {
      flag = 1;
    } else {
      flag = 0;
    }
  }

  if (count) {
    pos->length_args = count;
    pos->args = realloc(pos->args, pos->length_args * sizeof(char *));

    if (!(greps_f->f || greps_f->e)) {
      int shablon_size = strlen(pos->args[0]);
      pos->shablon_len += shablon_size;

      pos->shablons = realloc(pos->shablons, pos->shablon_len);
      strncat(pos->shablons, pos->args[0], pos->shablon_len);

      memmove(pos->args, pos->args + 1,
              (pos->length_args - 1) * sizeof(char *));
      --pos->length_args;
    }
  } else {
    greps_f->err = 1;
  }
}

void get_shablon_from_file(pos *pos, char *file_name, Flags *greps_f) {
  FILE *file_sh = fopen(file_name, "r");

  if (file_sh != NULL) {
    char file_buff[SIZE];

    while (!feof(file_sh)) {
      if (fgets(file_buff, SIZE, file_sh) != NULL) {
        int last_index = strlen(file_buff) - 1;

        if (file_buff[last_index] == '\n') {
          file_buff[last_index] = '\0';
        }

        shablon_cat(pos, file_buff);
      }
    }

    fclose(file_sh);
  } else {
    if (!greps_f->s) {
      perror("s21_grep");
    }
    greps_f->err = 1;
  }
}

void handle_flags(pos *pos, Flags *greps_f, int argc, char *argv[]) {
  int opt;

  while ((opt = getopt_long(argc, argv, "e:ivcnlhsf:o", NULL, 0)) != -1) {
    switch (opt) {
      case 'e':
        greps_f->e = 1;
        shablon_cat(pos, optarg);
        break;
      case 'i':
        greps_f->i = 1;
        break;
      case 'v':
        greps_f->v = 1;
        break;
      case 'c':
        greps_f->c = 1;
        break;
      case 'n':
        greps_f->n = 1;
        break;
      case 'l':
        greps_f->l = 1;
        break;
      case 'h':
        greps_f->h = 1;
        break;
      case 's':
        greps_f->s = 1;
        break;
      case 'f':
        greps_f->f = 1;
        get_shablon_from_file(pos, optarg, greps_f);
        break;
      case 'o':
        greps_f->o = 1;
        break;
      case '?':
        greps_f->err = 1;
        break;
    }
  }

  if (greps_f->f || greps_f->e) {
    delete_last_pipe(pos->shablons);
  }
}

void shablon_cat(pos *pos, char *src) {
  int line_size = strlen(src);
  pos->shablon_len += line_size + 1;

  pos->shablons = realloc(pos->shablons, pos->shablon_len);
  strncat(pos->shablons, src, pos->shablon_len);

  pos->shablons[pos->shablon_len - 2] = '|';
}

void delete_last_pipe(char *shablon) {
  int length = strlen(shablon);

  if (length > 0 && shablon[length - 1] == '|') {
    shablon[length - 1] = '\0';
  }
}

void print_error() {
  fprintf(stderr,
          "Usage: grep [OPTION]... PATTERNS [FILE]...\nTry 'grep "
          "--help' for more information.");
}

void init_flags(Flags *flag) {
  flag->e = flag->i = flag->v = flag->c = flag->n = flag->l = flag->h =
      flag->s = flag->f = flag->o = flag->err = 0;
}

void init_shablons(pos *position, int argc) {
  position->shablon_len = 1;
  position->shablons = calloc(position->shablon_len, sizeof(char));

  position->length_args = argc - 1;
  position->args = malloc(sizeof(char *) * position->length_args);
}

void free_pos(pos *position) {
  if (position->shablons != NULL) {
    free(position->shablons);
  }

  if (position->args != NULL) {
    free(position->args);
  }
}