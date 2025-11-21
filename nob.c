#include <stdlib.h>
#define NOB_STRIP_PREFIX
#define NOB_IMPLEMENTATION
#include "nob.h"
#define FLAG_IMPLEMENTATION
#include "flag.h"

typedef struct {
    char **items;
    int    capacity;
    int    count;
} MY_Flags_args;

Cmd           cmd            = { 0 };

MY_Flags_args ARGS           = { 0 };

char         *BUILD_DIR      = "./build";
char         *SRC_DIR        = "./src";
char         *THIRDPARTY_DIR = "./thirdparty";
char         *BUILD_MAIN;

char         *WEB_DIR = "./.web";
char         *WEB_SRC = "./web_src";
char         *WEB_TOOLS_DIR = "./build_web_tools";

void          cmd_cc( ) {
#ifdef _WIN32
  cmd_append( &cmd, "clang++" );
#else
  cmd_append( &cmd, "g++" );
#endif

  cmd_append( &cmd, "-std=c++23", "-Wall", "-Wextra", "-Werror", "-lm" );
}

void usage( FILE *stream ) {
  fprintf( stream, "Usage: ./%s [OPTIONS]\n", flag_program_name( ) );
  fprintf( stream, "OPTIONS:\n" );
  flag_print_options( stream );
}

void build_raylib( ) {
  // Clean raylib first to ensure no WASM objects are mixed in
  cmd_append( &cmd, "make", "-C", temp_sprintf( "%s/raylib/src", THIRDPARTY_DIR ), "clean" );
  if ( !cmd_run( &cmd ) ) {
      nob_log(WARNING, "Failed to clean raylib (might be fine if first run)");
  }
  cmd.count = 0;

  cmd_append(
    &cmd, "make", "-C", temp_sprintf( "%s/raylib/src", THIRDPARTY_DIR ),
    "PLATFORM=PLATFORM_DESKTOP_GLFW", "RAYLIB_LIBTYPE=STATIC",
    temp_sprintf( "RAYLIB_RELEASE_PATH=../../../%s", BUILD_DIR ) );
  if ( !cmd_run( &cmd ) ) {
    exit( 1 );
  }
  cmd.count = 0;
}

void setup_emscripten(const char *web_tools_dir) {
    if (!mkdir_if_not_exists(web_tools_dir)) {
        exit(1);
    }

    char *emsdk_dir = temp_sprintf("%s/emsdk", web_tools_dir);

    if (!file_exists(emsdk_dir)) {
        nob_log(INFO, "Emscripten SDK not found. Cloning...");
        cmd_append(&cmd, "git", "clone", "https://github.com/emscripten-core/emsdk.git", emsdk_dir);
        if (!cmd_run(&cmd)) exit(1);
        cmd.count = 0;

        nob_log(INFO, "Installing latest Emscripten...");
        cmd_append(&cmd, temp_sprintf("%s/emsdk", emsdk_dir), "install", "latest");
        if (!cmd_run(&cmd)) exit(1);
        cmd.count = 0;

        nob_log(INFO, "Activating latest Emscripten...");
        cmd_append(&cmd, temp_sprintf("%s/emsdk", emsdk_dir), "activate", "latest");
        if (!cmd_run(&cmd)) exit(1);
        cmd.count = 0;
    }
}

void build_web() {


  setup_emscripten(WEB_TOOLS_DIR);

  // Construct path to emcc
  // emsdk usually puts emcc in upstream/emscripten/emcc
  char *emcc_path = temp_sprintf("%s/emsdk/upstream/emscripten/emcc", WEB_TOOLS_DIR);

  // We need to add emsdk paths to PATH environment variable for emcc to work properly
  // or source emsdk_env.sh. Since we are in C, we can try setting PATH.
  // However, emcc needs a lot of env vars.
  // The easiest way is to use 'emsdk run' or source the script.
  // But 'emsdk run' might not be available in older versions? It is available now.
  // Let's try to use the absolute path to emcc first, it might complain about missing nodes etc.
  // A robust way is to generate a shell script that sources env and runs the command,
  // or use 'emsdk_env.sh' to set ENV in current process (hard in C).

  // Alternative: Use 'source ./emsdk_env.sh && emcc ...' via sh -c

  // Create .web directory
  if (!mkdir_if_not_exists(WEB_DIR)) {
    exit(1);
  }

  // Copy web_src content to .web
  // We use bash cp to ensure contents are copied to root of .web
  cmd_append(&cmd, "bash", "-c", temp_sprintf("cp -R %s/. %s/", WEB_SRC, WEB_DIR));
  if (!cmd_run(&cmd)) {
      nob_log(ERROR, "Failed to copy web_src to .web directory.");
      exit(1);
  }
  cmd.count = 0;

  // Copy assets to .web
  if (!copy_directory_recursively("assets", temp_sprintf("%s/assets", WEB_DIR))) {
    nob_log(ERROR, "Failed to copy assets to web directory.");
    exit(1);
  }

  // Build raylib for web
  // We need emcc in PATH for make to work with PLATFORM_WEB if it calls emcc directly.
  // Raylib's Makefile uses CC=emcc.
  // We can pass CC=/absolute/path/to/emcc

  // Build raylib
  // We wrap the make command in a shell to source the environment
  char *emsdk_env_cmd = temp_sprintf("source %s/emsdk/emsdk_env.sh && ", WEB_TOOLS_DIR);
  // Clean raylib first to ensure no native objects are mixed in
  char *clean_cmd = temp_sprintf("%s make -C %s/raylib/src clean", emsdk_env_cmd, THIRDPARTY_DIR);
  cmd_append(&cmd, "bash", "-c", clean_cmd);
  if (!cmd_run(&cmd)) {
    nob_log(WARNING, "Failed to clean raylib (might be fine if first run)");
    // Don't exit, just continue
  }
  cmd.count = 0;

  char *make_cmd = temp_sprintf("%s make -C %s/raylib/src PLATFORM=PLATFORM_WEB RAYLIB_LIBTYPE=STATIC RAYLIB_RELEASE_PATH=../../../%s CC=emcc AR=emar",
                                emsdk_env_cmd, THIRDPARTY_DIR, WEB_DIR);

  cmd_append(&cmd, "bash", "-c", make_cmd);
  if (!cmd_run(&cmd)) {
    exit(1);
  }
  cmd.count = 0;

  // Build with emcc
  // We also wrap this in bash to have the environment
  // Regenerate env cmd string to be safe against temp_sprintf buffer rotation
  emsdk_env_cmd = temp_sprintf("source %s/emsdk/emsdk_env.sh && ", WEB_TOOLS_DIR);

  char *build_cmd = temp_sprintf("%s emcc -o %s/index.html %s/main.cpp -I%s -I%s/raylib/src -I%s/raylib/src/external -L%s -lraylib.web -s USE_GLFW=3 -s ASYNCIFY -s ALLOW_MEMORY_GROWTH=1 -s INITIAL_MEMORY=134217728 -DPLATFORM_WEB --shell-file %s/shell.html --preload-file %s/assets@assets -O3",
    emsdk_env_cmd,
    WEB_DIR,
    SRC_DIR,
    SRC_DIR,
    THIRDPARTY_DIR,
    THIRDPARTY_DIR,
    WEB_DIR,
    WEB_DIR,
    WEB_DIR
  );

  cmd_append(&cmd, "bash", "-c", build_cmd);

  if (!cmd_run(&cmd)) {
    exit(1);
  }
  cmd.count = 0;

  nob_log(INFO, "Web build complete! Output in %s/", WEB_DIR);
  nob_log(INFO, "Test locally: cd %s && python3 -m http.server 8000", WEB_DIR);
}

int main( int argc, char **argv ) {

  da_append_many( &ARGS, argv, argc );

  NOB_GO_REBUILD_URSELF( argc, argv );

  bool help  = false;

  bool debug = false;

  bool run   = false;

  bool build = false;

  bool clean = false;

  bool web   = false;

  flag_bool_var( &help, "-help", false, "Print this help" );
  flag_bool_var( &help, "h", false, "Print this help" );
  flag_bool_var( &run, "r", false, "Run build" );
  flag_bool_var( &run, "-run", false, "Run build" );
  flag_bool_var( &build, "b", false, "Build" );
  flag_bool_var( &build, "-build", false, "Build" );
  flag_bool_var( &debug, "d", false, "enable debug mode" );
  flag_bool_var( &debug, "-debug", false, "enable debug mode" );
  flag_bool_var( &clean, "-clean", false, "Clean build directory" );
  flag_bool_var( &web, "w", false, "Build for web (WASM)" );
  flag_bool_var( &web, "-web", false, "Build for web (WASM)" );

  BUILD_MAIN = nob_temp_sprintf( "%s/cbps-lwp", BUILD_DIR );

  if ( !flag_parse( ARGS.count, ARGS.items ) ) {
    usage( stderr );
    flag_print_error( stderr );
    exit( 1 );
  }

  argc = flag_rest_argc( );
  argv = flag_rest_argv( );

  if ( help ) {
    usage( stdout );
    exit( 0 );
  }
  if ( build ) {

    if ( !mkdir_if_not_exists( BUILD_DIR ) ) {
      return 1;
    }

      if ( !copy_directory_recursively( "assets", temp_sprintf( "%s/assets", BUILD_DIR ) ) ) {
          nob_log(ERROR, "Failed to copy assets directory.\n");
          return 1;
      }


    build_raylib( );

    cmd_cc( );
    cmd_append( &cmd, "-o", BUILD_MAIN );
    cmd_append( &cmd, temp_sprintf( "-I%s", SRC_DIR ) );
    cmd_append( &cmd, temp_sprintf( "-L%s/", BUILD_DIR ) );
    cmd_append( &cmd, "-I", temp_sprintf( "%s/raylib/src", THIRDPARTY_DIR ) );
    cmd_append( &cmd, "-I", temp_sprintf( "%s/raygui/src", THIRDPARTY_DIR ) );
    cmd_append(
      &cmd, "-I", temp_sprintf( "%s/raylib/src/external", THIRDPARTY_DIR ) );
    cmd_append(
      &cmd, "-L/opt/homebrew/opt/glfw/lib",
      "-I/opt/homebrew/opt/glfw/include" );
    if ( debug ) {
      cmd_append( &cmd, "-g", "-O0" );
    } else {
      cmd_append( &cmd, "-O3" );
    }
    cmd_append( &cmd, temp_sprintf( "%s/main.cpp", SRC_DIR ) );
    cmd_append( &cmd, "-lraylib", "-lglfw" );
#ifdef __APPLE__
    cmd_append( &cmd, "-framework", "CoreFoundation" );
    cmd_append( &cmd, "-framework", "IOKit" );
    cmd_append( &cmd, "-framework", "AppKit" );
    cmd_append( &cmd, "-framework", "CoreGraphics" );
    cmd_append( &cmd, "-framework", "OpenGL" );
#endif
    if ( !cmd_run( &cmd ) ) {
      return 1;
    }
    cmd.count = 0;
  }

  if ( run ) {
    cmd_append( &cmd, BUILD_MAIN );
    if ( !cmd_run( &cmd ) ) {
      return 1;
    }
    cmd.count = 0;
  }

  if ( web ) {
    build_web();
  }

  if ( clean ) {
#ifdef _WIN32
    cmd_append( &cmd, "rmdir", "/s", "/q", BUILD_DIR, WEB_DIR, WEB_TOOLS_DIR );
#else
    cmd_append( &cmd, "rm", "-rf", BUILD_DIR, WEB_DIR, WEB_TOOLS_DIR );
#endif
    if ( !cmd_run( &cmd ) ) {
      return 1;
    }
    cmd.count = 0;
  }

  return 0;
}
