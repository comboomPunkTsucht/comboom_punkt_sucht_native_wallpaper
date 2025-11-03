#include <iostream>
  #include <chrono>
#include <cmath>
#include <algorithm>
#include <string>
#include <cctype>

  #include "../thirdparty/raylib/src/raylib.h"

  #define FLAG_IMPLEMENTATION
  #include "../thirdparty/flag.h/flag.h"
  #define CBPS_CONSTANTS_IMPLEMENTATION
  #include "./constants.hpp"
  #define NORD_COLORS_IMPLEMENTATION
  #include "./nord.hpp"

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

  namespace CBPS {

    typedef struct {
    float base_x;
    float base_y; // unveränderliche Ankerposition im Raum
    float ox;     // geglätteter Parallax-/Wackel-Offset (x)
    float oy;     // geglätteter Parallax-/Wackel-Offset (y)
    float vx;     // kleine Wackelgeschwindigkeit x
    float vy;     // kleine Wackelgeschwindigkeit y
    float parallax; // 0.2 .. 1.0, kleiner = weiter weg (weniger Bewegung)
    int   radius;
    Color color;
  } CirclePoint;
  CirclePoint circle_points[COUNT_CIRCLE_POINTS];
  }

  #ifdef CBPS_CONSTANTS_IMPLEMENTATION

namespace CBPS {

 void init_circle_points( int width, int height ) {
    for ( int i = 0; i < COUNT_CIRCLE_POINTS; i++ ) {
      float px = static_cast<float>( GetRandomValue(0, (int)width) );
      float py = static_cast<float>( GetRandomValue(0, (int)height) );
      circle_points[i].base_x = px;
      circle_points[i].base_y = py;
      circle_points[i].ox = 0.0f;
      circle_points[i].oy = 0.0f;
      circle_points[i].vx = GetRandomValue(-10, 10) / 100.0f; // very small jitter
      circle_points[i].vy = GetRandomValue(-10, 10) / 100.0f;
      circle_points[i].parallax = GetRandomValue(20, 100) / 100.0f; // 0.20 .. 1.00
      circle_points[i].radius = GetRandomValue(3, 6);
      circle_points[i].color = (GetRandomValue(0, 1) == 0)
          ? Nord::index_to_Color( Nord::NORD11 )
          : Nord::index_to_Color( Nord::NORD14 );
	circle_points[i].color.a = GetRandomValue(0x33, 0xCC); // etwas transparent
    }
  }

  void draw_circle_points() {
    // DrawRectangle(0, 0, GetRenderWidth(), GetRenderHeight(), index_to_Color( Nord::NORD0 ) );
    for ( int i = 0; i < COUNT_CIRCLE_POINTS; i++ ) {
      float draw_x = circle_points[i].base_x + circle_points[i].ox;
      float draw_y = circle_points[i].base_y + circle_points[i].oy;
      DrawCircleV( { draw_x, draw_y },
                   static_cast<float>(circle_points[i].radius),
                   circle_points[i].color );
    }
  }

  void update_circle_points(int width, int height, int /*unused*/ = 0) {
    const float dt = GetFrameTime();
    const float fps_norm = static_cast<float>( GetFPS()); // für framerate-unabhängige Bewegung
    const float jitter_strength = 0.03f;   // wie stark das zufällige Wackeln ist
    const float mouse_influence = 1.0f;    // skaliert Mauswirkung
    const float stiffness = 0.12f;         // wie schnell ox/oy zum Ziel konvergiert
    const float damping = 0.92f;           // für vx/vy Dämpfung
    const float max_offset_ratio = 0.04f;  // Anteil von min(width,height) als max pixel-offset
    const float max_jitter_speed = 0.8f;

    Vector2 mouse = GetMousePosition();
    float mx = 0.0f, my = 0.0f;
    if (width > 0 && height > 0) {
      mx = ((mouse.x / (float)width) - 0.5f) * 2.0f; // -1..1
      my = ((mouse.y / (float)height) - 0.5f) * 2.0f; // -1..1
    }

    float max_dim = static_cast<float>(std::min(width, height));
    float max_offset_px = max_dim * max_offset_ratio;

    for (int i = 0; i < COUNT_CIRCLE_POINTS; ++i) {
      // Zieloffset basierend auf Maus, gewichtbar durch Parallax:
      // Punkte mit kleinerer parallax bewegen sich weniger (weit weg).
      float influence = (1.0f - circle_points[i].parallax) * mouse_influence;
      float target_ox = mx * max_offset_px * influence;
      float target_oy = my * max_offset_px * influence;

      // Kleine zufällige Wackelbeschleunigung
      float ax = (GetRandomValue(-5, 5) / 5.0f) * jitter_strength * dt * fps_norm;
      float ay = (GetRandomValue(-5, 5) / 5.0f) * jitter_strength * dt * fps_norm;

      circle_points[i].vx += ax;
      circle_points[i].vy += ay;

      // Dämpfung für Jitter
      circle_points[i].vx *= damping;
      circle_points[i].vy *= damping;

      // Begrenze Jitter-Geschwindigkeit
      float jspeed = std::sqrt(circle_points[i].vx * circle_points[i].vx +
                               circle_points[i].vy * circle_points[i].vy);
      if (jspeed > max_jitter_speed && jspeed > 0.0f) {
        float s = max_jitter_speed / jspeed;
        circle_points[i].vx *= s;
        circle_points[i].vy *= s;
      }

      // Ox/oy nähert sich dem Zieloffset; zusätzlich wird Jitter addiert
      circle_points[i].ox += (target_ox - circle_points[i].ox) * stiffness;
      circle_points[i].oy += (target_oy - circle_points[i].oy) * stiffness;

      circle_points[i].ox += circle_points[i].vx;
      circle_points[i].oy += circle_points[i].vy;

      // Sicherstellen, dass gezeichnete Position nicht außerhalb des Fensters wandert
      float draw_x = circle_points[i].base_x + circle_points[i].ox;
      float draw_y = circle_points[i].base_y + circle_points[i].oy;
      draw_x = std::clamp(draw_x, 0.0f, static_cast<float>(width));
      draw_y = std::clamp(draw_y, 0.0f, static_cast<float>(height));

      // Wenn clamp gezogen wurde, passe ox/oy so an, dass base + ox/oy == geclampte Position
      circle_points[i].ox = draw_x - circle_points[i].base_x;
      circle_points[i].oy = draw_y - circle_points[i].base_y;
    }
  }

void print_usage( FILE *stream = stdout ) {
    std::fprintf( stream, "Usage: %s [Options]\n", flag_program_name( ) );
    print_app_info( stdout );
    std::printf( "\nOptions:\n" );
    flag_print_options( stdout );
  }


 // Helper: safer lowercase conversion for C-strings
std::string toLowerStr(const char *s) {
    std::string r = s ? std::string(s) : std::string();
    std::transform(r.begin(), r.end(), r.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    return r;
  }

} // namespace CBPS

  #endif  // CBPS_CONSTANTS_IMPLEMENTATION


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

    // Seed einmalig hier, bevor Punkte initialisiert werden
    {
      auto now = std::chrono::system_clock::now();
      auto seed = static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
      SetRandomSeed(seed);
    }

    CBPS::init_circle_points( window_width, window_height );

    // Initialize raylib window
    SetConfigFlags( FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_INTERLACED_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_ALWAYS_RUN | FLAG_WINDOW_MAXIMIZED | FLAG_WINDOW_UNFOCUSED );
    InitWindow( window_width, window_height, CBPS::APP_NAME );
    SetWindowMinSize( CBPS::MIN_WINDOW_WIDTH, CBPS::MIN_WINDOW_HEIGHT );
    SetTargetFPS( GetMonitorRefreshRate( GetCurrentMonitor() ) >= 60 ? GetMonitorRefreshRate( GetCurrentMonitor() ) : 60 );


    // Texture2D fields: id, width, height, mipmaps, format
    Texture2D cbpsTex = {0,0,0,0,0};
    Texture2D mahdTex = {0,0,0,0,0};
    Texture2D bdTex = {0,0,0,0,0};
    Texture2D knudTex = {0,0,0,0,0};
    Texture2D fabelTex = {0,0,0,0,0};
    bool cbpsLoaded = false, mahdLoaded = false, bdLoaded = false, knudLoaded = false, fabelLoaded = false;
    cbpsTex = LoadTexture( "./assets/images/cbps_logo.png" ); cbpsLoaded = (cbpsTex.id != 0);
    mahdTex = LoadTexture( "./assets/images/mahd_logo.png" ); mahdLoaded = (mahdTex.id != 0);
    bdTex = LoadTexture( "./assets/images/bd_logo.png" ); bdLoaded = (bdTex.id != 0);
    knudTex = LoadTexture( "./assets/images/knudelzwerck_logo.png" ); knudLoaded = (knudTex.id != 0);
    fabelTex = LoadTexture( "./assets/images/fabelke_logo.png" ); fabelLoaded = (fabelTex.id != 0);


	// Font

	Font titleFont = LoadFontEx( CBPS::FONT_PATH, 160, NULL, CBPS::GLYPHS_COUNT );
	if (!IsFontValid(titleFont)) {
		fprintf(stderr ,"Failed to load font!\n");
    	return 1;
	}
	Font subFont = LoadFontEx( CBPS::FONT_PATH, 48, NULL, CBPS::GLYPHS_COUNT );
		if (!IsFontValid(titleFont)) {
		fprintf(stderr ,"Failed to load font!\n");
    return 1;
	}

    while ( !WindowShouldClose( ) ) {
      if ( IsWindowResized( ) ) {
        window_width = GetRenderWidth( );
        window_height = GetRenderHeight( );
        CBPS::init_circle_points( window_width, window_height );
      }
      BeginDrawing( );
      ClearBackground( Nord::index_to_Color( Nord::NORD0 ) );
      CBPS::draw_circle_points( );

      // --- Draw centered title / subtitle and decorative lines (inspired by __root.tsx) ---

      // Dynamic font sizes based on window width
      int titleFontSize = std::clamp( (int)(window_width / 10), 24, 160 );
      int subFontSize = std::clamp( (int)(window_width / 30), 12, 48 );

	  if (titleFont.baseSize != titleFontSize) {
          UnloadFont(titleFont);
		  titleFont = LoadFontEx( CBPS::FONT_PATH, titleFontSize, NULL, CBPS::GLYPHS_COUNT );
          if (!IsFontValid(titleFont)) {
              fprintf(stderr ,"Failed to load font!\n");
              return 1;
          }
      }

	  if (subFont.baseSize != subFontSize) {
          UnloadFont(subFont);
          subFont = LoadFontEx( CBPS::FONT_PATH, subFontSize, NULL, CBPS::GLYPHS_COUNT );
          if (!IsFontValid(subFont)) {
              fprintf(stderr ,"Failed to load font!\n");
              return 1;
          }
      }

      // Measure title and subtitle using loaded fonts
      Vector2 titleMeasure = MeasureTextEx( titleFont, title, (float)titleFontSize, 1.0f );
      Vector2 subMeasure = MeasureTextEx( subFont, sub_title, (float)subFontSize, 1.0f );

      float centerX = (float)window_width * 0.5f;
      float centerY = (float)window_height * 0.5f;

      float titleX = centerX - titleMeasure.x * 0.5f;
      float titleY = centerY - titleMeasure.y * 0.6f; // slightly above center

      float subX = centerX - subMeasure.x * 0.5f;
      float subY = titleY + titleMeasure.y + 24.0f; // spacing below title

      // Decorative lines: thin gradient rectangles above and below the title
      float lineThickness = 3.0f;
      float lineWidth = (float)window_width * 0.85f; // cover most of the width
      float lineLeft = centerX - lineWidth * 0.5f;

      Color colA = Nord::index_to_Color( Nord::NORD0 );
      Color colB = Nord::index_to_Color( Nord::NORD6 );

      // Draw top line as A -> B -> A by drawing two halves
      int halfWidth = (int)(lineWidth * 0.5f);
      int leftX = (int)lineLeft;
      DrawRectangleGradientH(leftX, (int)(titleY - 28.0f), halfWidth, (int)lineThickness, colA, colB);
      DrawRectangleGradientH(leftX + halfWidth, (int)(titleY - 28.0f), (int)lineWidth - halfWidth, (int)lineThickness, colB, colA);
      // Draw bottom line as A -> B -> A
      DrawRectangleGradientH(leftX, (int)(titleY + titleMeasure.y + 8.0f), halfWidth, (int)lineThickness, colA, colB);
      DrawRectangleGradientH(leftX + halfWidth, (int)(titleY + titleMeasure.y + 8.0f), (int)lineWidth - halfWidth, (int)lineThickness, colB, colA);

      // Draw title centered
      DrawTextEx( titleFont, title, { titleX, titleY }, (float)titleFontSize, 1.0f, Nord::index_to_Color( Nord::NORD6 ) );

      // If sub_title equals "comboom.sucht", draw the cbps logo instead of the text (centered)
      if ( sub_title != nullptr && std::string(sub_title) == "comboom.sucht" && cbpsLoaded ) {
        float targetH = std::clamp( (float)window_height * 0.12f, 48.0f, 192.0f );
        float scale = targetH / (float)cbpsTex.height;
        float w = cbpsTex.width * scale;
        float x = centerX - w * 0.5f;
        float y = subY; // place where subtitle would be
        DrawTextureEx( cbpsTex, { x, y }, 0.0f, scale, Nord::index_to_Color( Nord::NORD6 ) );
      } else {
        DrawTextEx( subFont, sub_title, { subX, subY }, (float)subFontSize, 1.0f, Nord::index_to_Color( Nord::NORD6 ) );
      }

      // Determine optional small logo for the title (bottom-right), based on title string
      std::string titleLower = CBPS::toLowerStr( title );
      Texture2D *h1Logo = nullptr;
      bool h1LogoLoaded = false;
      if ( titleLower == "mcpeaps_hd" ) { h1Logo = &mahdTex; h1LogoLoaded = mahdLoaded; }
      else if ( titleLower == "blackdragon" ) { h1Logo = &bdTex; h1LogoLoaded = bdLoaded; }
      else if ( titleLower == "knuddelzwerck" ) { h1Logo = &knudTex; h1LogoLoaded = knudLoaded; }
      else if ( titleLower == "fabelke" ) { h1Logo = &fabelTex; h1LogoLoaded = fabelLoaded; }

      if ( h1Logo != nullptr && h1LogoLoaded ) {
        float padding = 16.0f;
        float targetH = std::clamp( (float)window_height * 0.06f, 24.0f, 128.0f );
        float scale = targetH / (float)h1Logo->height;
        float w = h1Logo->width * scale;
        float h = h1Logo->height * scale;
        float x = (float)window_width - padding - w;
        float y = (float)window_height - padding - h;
        DrawTextureEx( *h1Logo, { x, y }, 0.0f, scale, WHITE );
      }

      // -------------------------------------------------------------------------------

      //DrawText( title, 10, 10, 20, index_to_Color( Nord::NORD5 ) );
      //DrawText( sub_title, 10, 40, 15, index_to_Color( Nord::NORD5 ) );
	  //DrawText( std::to_string(GetFPS()).c_str(), 10, 70, 15, index_to_Color( Nord::NORD5 ) );
	  //printf( "FPS: %d\n", GetFPS() );

      EndDrawing( );

      CBPS::update_circle_points( window_width, window_height );
    }

    // Free loaded fonts and textures
    UnloadFont( titleFont );
    UnloadFont( subFont );
    if ( cbpsLoaded ) UnloadTexture( cbpsTex );
    if ( mahdLoaded ) UnloadTexture( mahdTex );
    if ( bdLoaded ) UnloadTexture( bdTex );
    if ( knudLoaded ) UnloadTexture( knudTex );
    if ( fabelLoaded ) UnloadTexture( fabelTex );

    CloseWindow( );

    return 0;
  }