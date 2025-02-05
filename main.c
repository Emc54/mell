#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MELL_RL_BUFFSIZE 1024;
// #define DEBUG ;

void
mell_loop(void);

int
main(int argc, char** argv)
{

  // load config files if any

  // main loop
  mell_loop();

  // exit
  return EXIT_SUCCESS;
}

char*
mell_read_line(void)
{

  int bufsize = MELL_RL_BUFFSIZE;
  int position = 0;
  char* buffer = calloc(sizeof(char) * bufsize, 1);
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
#ifdef DEBUG
      printf("[DEBUG] mell_read_line returned: %s \r\n", buffer);
#endif /* ifdef DEBUG */
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

char*
mell_getline(void)
{
  char* lineptr = NULL;
  size_t buffline = 0;

  if (getline(&lineptr, &buffline, stdin) == -1) {
    if (feof(stdin)) {
      printf("success"); // exit(EXIT_SUCCESS);
    } else {
      perror("readline");
      exit(EXIT_FAILURE);
    }
  }

#ifdef DEBUG
  printf("[DEBUG] Getline returned: %s \r\n", lineptr);
#endif /* ifdef DEBUG */

  return lineptr;
}

#define MELL_TOK_BUFSIZE 64
#define MELL_TOK_DELIM " \t\r\n\a"

char**
mell_split_line(char* line)
{

  int bufsize = MELL_TOK_BUFSIZE;
  int position = 0;

  char** tokens = malloc(bufsize * sizeof(char*));
  char* token;

  if (!tokens) {
    fprintf(stderr, "mell: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, MELL_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += MELL_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));

      if (!tokens) {
        fprintf(stderr, "mell: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, MELL_TOK_DELIM);
  }

  tokens[position] = NULL;
  return tokens;
}

int
launch(char** args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();

  if (pid == 0) {
    if (execvp(args[0], args) == -1) {
      perror("mell");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    perror("mell");
  } else {
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  return 1;
}

/*
 * Builtin shell commands
 */

int
mell_cd(char** args);
int
mell_help(char** args);
int
mell_exit(char** args);

char* builtin_str[] = { "cd", "help", "exit" };
int (*builtin_func[])(char**) = { &mell_cd, &mell_help, &mell_exit };

int
mell_cd(char** args)
{

  if (args[1] == NULL || strcmp(args[1], "~") == 0) {
    const char* home = getenv("HOME");
    chdir(home);
  } else if (chdir(args[1]) != 0) {
    perror("mell");
  }
  return 1;
}

int
mell_help(char** args)
{
  printf("Custom Shell for learning\n");
  printf("The following are built in:\n");

  for (int i = 0; i < sizeof(builtin_str) / sizeof(char*); i++) {
    printf(" %s\n", builtin_str[i]);
  }
  return 1;
}

int
mell_exit(char** args)
{
  return 0;
}

int
mell_execute(char** args)
{

  if (args[0] == NULL) {
    return 1;
  }

  for (int i = 0; i < sizeof(builtin_str) / sizeof(char*); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return launch(args);
}

void
mell_loop(void)
{

  char* line;
  char** args;
  int status;

  do {
    printf("mell> ");
    // line = mell_read_line();
    line = mell_getline();

#ifdef DEBUG
    printf("[DEBUG] User Input: %s \n", line);
#endif /* ifdef DEBUG */

    args = mell_split_line(line);

#ifdef DEBUG
    printf("[DEBUG] Tokens:\n");
    for (int i = 0; args[i] != NULL; i++)
      printf("[token %d)] %s \n", i + 1, args[i]);
#endif /* ifdef DEBUG */

    status = mell_execute(args);

    free(line);
    free(args);
  } while (status);
}
