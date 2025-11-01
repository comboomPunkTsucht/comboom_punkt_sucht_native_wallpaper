// std c++23
#include <iostream>

#include "../thirdparty/raylib/src/raylib.h"

#define FLAG_IMPLEMENTATION
#include "../thirdparty/flag.h/flag.h"
#define CBPS_CONSTANTS_IMPLEMENTATION
#include "./constants.hpp"
#define NORD_COLORS_IMPLEMENTATION
#include "./nord.hpp"

#ifdef CBPS_CONSTANTS_IMPLEMENTATION

namespace CBPS {
  void print_usage( FILE *stream = stdout ) {
    std::fprintf( stream, "Usage: %s [Options]\n", flag_program_name( ) );
    print_app_info( stdout );
    std::printf( "\nOptions:\n" );
    flag_print_options( stdout );
  }
}  // namespace CBPS

#endif  // CBPS_CONSTANTS_IMPLEMENTATION

#ifdef NORD_COLORS_IMPLEMENTATION

namespace Nord {
  Color index_to_Color( NordColor color ) {

    return Color{
      .r = static_cast< unsigned char >(
        ( index_to_uint32( color ) >> 24 ) & 0xFF ),
      .g = static_cast< unsigned char >(
        ( index_to_uint32( color ) >> 16 ) & 0xFF ),
      .b = static_cast< unsigned char >(
        ( index_to_uint32( color ) >> 8 ) & 0xFF ),
      .a = static_cast< unsigned char >( index_to_uint32( color ) & 0xFF ) };
  }
}  // namespace Nord

#endif  // NORD_COLORS_IMPLEMENTATION

int main( int argc, char *argv[] ) {
  // Parse command line flags using flag.h
  bool     show_version   = false;
  bool     show_V_version = false;
  bool     show_help      = false;
  bool     show_h_help    = false;
  char    *title          = ( char * ) CBPS::DEFAULT_TITLE;
  char    *sub_title      = ( char * ) CBPS::DEFAULT_SUB_TITLE;
  uint64_t window_width   = CBPS::DEFAULT_WINDOW_WIDTH;
  uint64_t window_height  = CBPS::DEFAULT_WINDOW_HEIGHT;

  flag_bool_var( &show_version, "-version", false, "Show version information" );
  flag_bool_var(
    &show_V_version, "V", false, "Show version information (short)" );
  flag_bool_var( &show_help, "-help", false, "Show help information" );
  flag_bool_var( &show_h_help, "h", false, "Show help information (short)" );
  flag_str_var( &title, "-title", CBPS::DEFAULT_TITLE, "Set window title" );
  flag_str_var(
    &sub_title, "-sub_title", CBPS::DEFAULT_SUB_TITLE, "Set window sub title" );
  flag_uint64_var(
    &window_width, "-width", CBPS::DEFAULT_WINDOW_WIDTH, "Set window width" );
  flag_uint64_var(
    &window_height, "-height", CBPS::DEFAULT_WINDOW_HEIGHT,
    "Set window height" );

  if ( !flag_parse( argc, argv ) ) {
    flag_print_error( stderr );
    CBPS::print_usage( stderr );
    return 1;
  }

  if ( show_version || show_V_version ) {
    CBPS::print_app_version( );
    return 0;
  }

  if ( show_help || show_h_help ) {
    CBPS::print_usage( stdout );
    return 0;
  }

  // Initialize raylib window
  SetConfigFlags(                       /*FLAG_WINDOW_UNDECORATED |*/
                  FLAG_WINDOW_RESIZABLE /*|
                                           FLAG_WINDOW_MOUSE_PASSTHROUGH
                                         */
                  | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT
                  | FLAG_INTERLACED_HINT | FLAG_VSYNC_HINT
                  | FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_ALWAYS_RUN
                  | FLAG_WINDOW_MAXIMIZED | FLAG_WINDOW_UNFOCUSED );
  InitWindow( window_width, window_height, CBPS::APP_NAME );
  SetWindowMinSize( CBPS::MIN_WINDOW_WIDTH, CBPS::MIN_WINDOW_HEIGHT );
  SetTargetFPS( 60 );
  while ( !WindowShouldClose( ) ) {
    BeginDrawing( );
    ClearBackground( index_to_Color( Nord::NORD0 ) );

    DrawText( title, 10, 10, 20, index_to_Color( Nord::NORD5 ) );
    DrawText( sub_title, 10, 40, 15, index_to_Color( Nord::NORD5 ) );

    

    EndDrawing( );
  }

  CloseWindow( );

  return 0;
}
