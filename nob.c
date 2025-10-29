#include <stdlib.h>
#define NOB_STRIP_PREFIX
#define NOB_IMPLEMENTATION
#include "nob.h"
#define FLAG_IMPLEMENTATION
#include "flag.h"

typedef struct {
  char **items;
  int capacity;
  int count;
} MY_Flags_args;

Cmd cmd = {0};

MY_Flags_args ARGS = {0};

char *BUILD_DIR = "./build";
char *SRC_DIR = "./src";
char *BUILD_MAIN;

void cmd_cc() {
#ifdef _WIN32
  cmd_append(&cmd, "clang");
#else
  cmd_append(&cmd, "cc");
#endif

  cmd_append(&cmd, "-std=c11", "-Wall", "-Wextra", "-Wpedantic", "-Werror");
}

void usage(FILE *stream) {
  fprintf(stream, "Usage: ./example [OPTIONS] [--] [ARGS]\n");
  fprintf(stream, "OPTIONS:\n");
  flag_print_options(stream);
}

int main(int argc, char **argv) {

  da_append_many(&ARGS, argv, argc);

  NOB_GO_REBUILD_URSELF(argc, argv);

  bool help = false;
  bool hhelp = false;

  bool run_build = false;
  bool rrun_build = false;

  bool build = false;
  bool bbuild = false;

  bool clean = false;

  flag_bool_var(&help, "-help", false, "Print this help");
  flag_bool_var(&hhelp, "h", false, "Print this help");
  flag_bool_var(&rrun_build, "r", false, "Run build");
  flag_bool_var(&run_build, "-run", false, "Run build");
  flag_bool_var(&bbuild, "b", false, "Build");
  flag_bool_var(&build, "-build", false, "Build");
  flag_bool_var(&clean, "-clean", false, "Clean build directory");

  BUILD_MAIN = nob_temp_sprintf("%s/main", BUILD_DIR);

  if (!flag_parse(ARGS.count, ARGS.items)) {
    usage(stderr);
    flag_print_error(stderr);
    exit(1);
  }

  argc = flag_rest_argc();
  argv = flag_rest_argv();

  if (help || hhelp) {
    usage(stdout);
    exit(0);
  }
  if (build || bbuild) {

    if (!nob_mkdir_if_not_exists(BUILD_DIR))
      return 1;

    cmd_cc();
    cmd_append(&cmd, "-o", BUILD_MAIN);
    cmd_append(&cmd, temp_sprintf("-I%s", SRC_DIR));
    cmd_append(&cmd, temp_sprintf("%s/main.c", SRC_DIR));
    if (!cmd_run(&cmd))
      return 1;
    cmd.count = 0;
  }

  if (run_build || rrun_build) {
    cmd_append(&cmd, BUILD_MAIN);
    if (!cmd_run(&cmd))
      return 1;
    cmd.count = 0;
  }

  if (clean) {
    #ifdef _WIN32
    cmd_append(&cmd, "rmdir", "/s", "/q", BUILD_DIR);
    #else
    cmd_append(&cmd, "rm", "-rf", BUILD_DIR);
    #endif
    if (!cmd_run(&cmd))
      return 1;
    cmd.count = 0;
  }

  return 0;
}
