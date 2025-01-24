#include <stdio.h>
#include <stdlib.h>

#define MELL_RL_BUFFSIZE 1024;
#define DEBUG 1;

void mell_loop(void);

int main(int argc, char **argv) {

  // load config files if any

  // main loop
  mell_loop();

  // exit
  return EXIT_SUCCESS;
}

char *mell_read_line(void) {

  int bufsize = MELL_RL_BUFFSIZE;
  int position = 0;
  char *buffer = calloc(sizeof(char) * bufsize, 1);
  int character;

  if (!buffer) {
    fprintf(stderr, "mell: couldn't allocate buffer\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    character = getchar();

    // Handle EOF
    if (character == EOF || character == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = character;
    }

    position++;

    // re-allocate past buffer size
    if (position >= bufsize) {
      bufsize += MELL_RL_BUFFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "mell: couldn't re-allocate buffer\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

char **mell_split_line(char *line);
int mell_execute(char **args);

void mell_loop(void) {

  char *line;
  char **args;
  int status;

  do {
    printf("mell> ");
    line = mell_read_line();

#ifdef DEBUG
    printf("[DEBUG] User Input: %s \n", line);
#endif /* ifdef DEBUG */

    /*args = mell_split_line(line);
    status = mell_execute(args);*/

    args = 0;
    status = 0;
    free(line);
    free(args);
  } while (status);
}
