#include "s21_cat.h"

void init_flags_counter(flags *counter) {
  counter->temp_f.n = counter->temp_f.b = counter->temp_f.s =
      counter->temp_f.s1 = counter->temp_f.text = 0;
}

void init_flags(flags *flag) {
  flag->b = flag->e = flag->v = flag->n = flag->s = flag->t = flag->files = 0;
}

void encode_with_notation(int ch) {
  if (ch != 10) {
    if (ch >= 128) {
      ch -= 128;
      putchar('M');
      putchar('-');
    }

    if (ch < 32 || ch == 127) {
      putchar('^');
      ch ^= 0x40;
    }

    putchar(ch);
  } else if (ch == 10) {
    putchar('$');
    putchar('\n');
  }
}

int streams_getc(FILE *streams[], int count, int *stream_i) {
  int ch;

  while (1) {
    if (streams[*stream_i] == NULL) {
      perror("No such file or directory");
      ++*stream_i;
      continue;
    }

    ch = fgetc(streams[*stream_i]);

    if (ch == EOF) {
      if (*stream_i < count - 1) {
        ++*stream_i;
      } else {
        break;
      }
    } else {
      break;
    }
  }

  return ch;
}

void first_line_nb(flags *cats, FILE *streams[], int count, int *stream_i) {
  int temp;

  if (cats->n && cats->files) {
    printf("%6d\t", cats->line);
  } else if (cats->n && !cats->files &&
             (temp = streams_getc(streams, count, stream_i))) {
    printf("%6d\t", cats->line);

    if (temp == '\n') {
      cats->temp_f.n = 1;
      cats->line++;
    }
  }

  if (cats->b && (temp = streams_getc(streams, count, stream_i)) != '\n') {
    printf("%6d\t", cats->line);
    cats->line++;
    putchar(temp);
  } else if (temp == '\n') {
    cats->temp_f.b = 1;
    putchar(temp);
  }
}

int case_n(flags *cats, int temp) {
  int continue_itteration = 0;

  if (cats->temp_f.n && temp == '\n') {
    printf("%6d\t", cats->line);
    cats->line++;
    putchar(temp);
    continue_itteration = 1;
  }

  if (temp != '\n' && cats->temp_f.n && !continue_itteration) {
    printf("%6d\t", cats->line);
    cats->temp_f.n = 0;

  } else if (temp == '\n' && !cats->temp_f.n && !continue_itteration) {
    cats->line++;
    cats->temp_f.n = 1;
  }

  return continue_itteration;
}

void case_b(flags *cats, int temp) {
  if (temp == '\n' && !cats->temp_f.b) {
    cats->temp_f.b = 1;
  } else if (temp != '\n' && cats->temp_f.b) {
    printf("%6d\t", cats->line);
    cats->line++;
    cats->temp_f.b = 0;
  }
}

int case_s(flags *cats, int temp) {
  int continue_itteration = 0;

  if (temp == '\n' && !cats->temp_f.s) {
    cats->temp_f.s = 1;
  } else if (temp == '\n' && cats->temp_f.s) {
    if (!cats->temp_f.s1 && cats->temp_f.text) {
      cats->temp_f.s1 = 1;
      putchar(temp);
    }
    continue_itteration = 1;
  } else if (temp != '\n') {
    cats->temp_f.s = 0;
    cats->temp_f.s1 = 0;
    cats->temp_f.text = 1;
  }

  return continue_itteration;
}

void cases_v_e_t(flags *cats, int temp) {
  if (cats->v && (temp < 32 || temp > 126) && (temp != 9 && temp != 10)) {
    encode_with_notation(temp);
  } else if (cats->e && cats->v && (temp < 32 || temp > 126) && (temp != 9)) {
    encode_with_notation(temp);
  } else if (cats->e && !cats->v && temp == 10) {
    encode_with_notation(temp);
  } else if (cats->t && cats->v && (temp < 32 || temp > 126) && (temp != 10)) {
    encode_with_notation(temp);
  } else if (cats->t && temp == 9) {
    encode_with_notation(temp);
  } else {
    putchar(temp);
  }
}

void output_streams(flags *cats, FILE *streams[], int count) {
  int temp, stream_i = 0;

  init_flags_counter(cats);

  if (cats->n || cats->b) {
    first_line_nb(cats, streams, count, &stream_i);
  }

  while ((temp = streams_getc(streams, count, &stream_i)) != EOF) {
    if (cats->n && case_n(cats, temp)) {
      continue;
    }

    if (cats->b) {
      case_b(cats, temp);
    }

    if (cats->s && case_s(cats, temp)) {
      continue;
    }

    if (cats->v || cats->e || cats->t) {
      cases_v_e_t(cats, temp);
    } else {
      putchar(temp);
    }
  }
}

void handle_flags(flags *cats, int argc, char *argv[]) {
  int opt;
  cats->none = 0;

  while ((opt = getopt(argc, argv, "beEvnstT")) != -1) {
    switch (opt) {
    case 'b':
      cats->b = 1;
      cats->line = 1;
      cats->none = 1;
      break;
    case 'E':
      cats->e = 1;
      cats->none = 1;
      break;
    case 'e':
      cats->e = 1;
      cats->v = 1;
      cats->none = 1;
      break;
    case 'v':
      cats->v = 1;
      cats->none = 1;
      break;
    case 'n':
      cats->n = 1;
      cats->line = 1;
      cats->none = 1;
      break;
    case 's':
      cats->s = 1;
      cats->none = 1;
      break;
    case 't':
      cats->t = 1;
      cats->v = 1;
      cats->none = 1;
      break;
    case 'T':
      cats->t = 1;
      cats->none = 1;
      break;
    case '?':
      fprintf(stderr, "Try 'cat --help' for more information.");
      break;
    }
  }
}

void cat_with_files(flags *cats, int argc, char *argv[]) {
  FILE *files[argc];

  for (int i = 0; i < argc; i++) {
    files[i] = fopen(argv[i], "r");
  }

  output_streams(cats, files, argc);

  for (int i = 0; i < argc; i++) {
    fclose(files[i]);
  }
}

int get_pos_args(char *pos_args[], int argc, char *argv[]) {
  int count = 0;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] != '-') {
      pos_args[count] = argv[i];
      ++count;
    }
  }

  return count;
}

int main(int argc, char *argv[]) {
  flags cats;
  init_flags(&cats);

  if (argc > 1) {
    handle_flags(&cats, argc, argv);

    char *pos_args[argc - 1];
    cats.files = get_pos_args(pos_args, argc, argv);

    if (cats.none) {
      if (cats.files != 0) {
        cat_with_files(&cats, cats.files, pos_args);
      } else {
        FILE *streams[1] = {stdin};
        output_streams(&cats, streams, 1);
      }
    } else {
      cat_with_files(&cats, cats.files, pos_args);
    }

  } else {
    FILE *streams[1] = {stdin};
    output_streams(&cats, streams, 1);
  }

  return 0;
}