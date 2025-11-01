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
  cmd_append(
    &cmd, "make", "-C", temp_sprintf( "%s/raylib/src", THIRDPARTY_DIR ),
    "PLATFORM=PLATFORM_DESKTOP_GLFW", "RAYLIB_LIBTYPE=STATIC",
    temp_sprintf( "RAYLIB_RELEASE_PATH=../../../%s", BUILD_DIR ) );
  if ( !cmd_run( &cmd ) ) {
    exit( 1 );
  }
  cmd.count = 0;
}

int main( int argc, char **argv ) {

  da_append_many( &ARGS, argv, argc );

  NOB_GO_REBUILD_URSELF( argc, argv );

  bool help  = false;

  bool debug = false;

  bool run   = false;

  bool build = false;

  bool clean = false;

  flag_bool_var( &help, "-help", false, "Print this help" );
  flag_bool_var( &help, "h", false, "Print this help" );
  flag_bool_var( &run, "r", false, "Run build" );
  flag_bool_var( &run, "-run", false, "Run build" );
  flag_bool_var( &build, "b", false, "Build" );
  flag_bool_var( &build, "-build", false, "Build" );
  flag_bool_var( &debug, "d", false, "enable debug mode" );
  flag_bool_var( &debug, "-debug", false, "enable debug mode" );
  flag_bool_var( &clean, "-clean", false, "Clean build directory" );

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

    if ( !nob_mkdir_if_not_exists( BUILD_DIR ) ) {
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

  if ( clean ) {
#ifdef _WIN32
    cmd_append( &cmd, "rmdir", "/s", "/q", BUILD_DIR );
#else
    cmd_append( &cmd, "rm", "-rf", BUILD_DIR );
#endif
    if ( !cmd_run( &cmd ) ) {
      return 1;
    }
    cmd.count = 0;
  }

  return 0;
}
