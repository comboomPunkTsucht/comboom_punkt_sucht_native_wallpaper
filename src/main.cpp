#include <iostream>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <string>
#include <cctype>

  #include "../thirdparty/raylib/src/raylib.h"

  #ifndef PLATFORM_WEB
  #define FLAG_IMPLEMENTATION
  #include "../thirdparty/flag.h/flag.h"
  #endif
  #define CBPS_CONSTANTS_IMPLEMENTATION
  #include "./constants.hpp"
  #define NORD_COLORS_IMPLEMENTATION
  #include "./nord.hpp"
  #ifndef PLATFORM_WEB
  #include "./desktop_wallpaper.hpp"
  #endif

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
    float x;           // aktuelle Position x
    float y;           // aktuelle Position y
    float translateX;  // Mouse-Offset x (smooth)
    float translateY;  // Mouse-Offset y (smooth)
    float dx;          // konstante Drift-Geschwindigkeit x
    float dy;          // konstante Drift-Geschwindigkeit y
    float magnetism;   // 0.1 .. 4.1, Anziehung zur Maus
    int   radius;
    Color color;
  } CirclePoint;
  CirclePoint circle_points[COUNT_CIRCLE_POINTS];
  }

  #ifdef CBPS_CONSTANTS_IMPLEMENTATION

namespace CBPS {

  Vector2 get_random_pos() {
    int x, y;
    do {

      x = GetRandomValue(-1, 1);
      y = GetRandomValue(-1, 1);

    } while((x == 0) && (y == 0)); // Wiederhole solange BEIDE 0 sind

    x *= GetRandomValue(1, CBPS::MAX_RANDOM_FAKTOR);
    y *= GetRandomValue(1, CBPS::MAX_RANDOM_FAKTOR);

    return (Vector2){
      .x = x * 1.0f,
      .y = y * 1.0f
    };
  }

 void init_circle_points( int width, int height ) {
    for ( int i = 0; i < COUNT_CIRCLE_POINTS; i++ ) {
      // Position: irgendwo auf der Canvas
      circle_points[i].x = static_cast<float>( GetRandomValue(0, width) );
      circle_points[i].y = static_cast<float>( GetRandomValue(0, height) );

      // Mouse-Offset startet bei 0
      circle_points[i].translateX = 0.0f;
      circle_points[i].translateY = 0.0f;

      // Konstante Drift: (random - 0.5) * 0.2 → -0.1 bis +0.1
      circle_points[i].dx = (GetRandomValue(0, 100) / 100.0f - 0.5f) * 0.2f;
      circle_points[i].dy = (GetRandomValue(0, 100) / 100.0f - 0.5f) * 0.2f;

      // Magnetism: 0.1 + random * 4.0 → 0.1 bis 4.1
      circle_points[i].magnetism = 0.1f + (GetRandomValue(0, 400) / 100.0f);

      // Größe: 5 bis 8 (größer für bessere Sichtbarkeit)
      circle_points[i].radius = GetRandomValue(3, 5);

      // Farbe: Nord11 (rot) oder Nord14 (grün)
      circle_points[i].color = (GetRandomValue(0, 1) == 0)
          ? Nord::index_to_Color( Nord::NORD11 )
          : Nord::index_to_Color( Nord::NORD14 );
      // Alpha: 150 bis 230 (0.6 bis 0.9 in 0-255) - deutlich sichtbarer
      circle_points[i].color.a = GetRandomValue(150, 230);
    }
  }

  void draw_circle_points(Shader *shader = nullptr) {
    if (shader != nullptr && shader->id != 0) {
      BeginShaderMode(*shader);
    }

    for ( int i = 0; i < COUNT_CIRCLE_POINTS; i++ ) {
      // Draw position = base position + mouse offset
      float draw_x = circle_points[i].x + circle_points[i].translateX;
      float draw_y = circle_points[i].y + circle_points[i].translateY;
      DrawCircleV( { draw_x, draw_y },
                   static_cast<float>(circle_points[i].radius),
                   circle_points[i].color );
    }

    if (shader != nullptr && shader->id != 0) {
      EndShaderMode();
    }
  }

  void update_circle_points(int width, int height, int /*unused*/ = 0) {
    // TypeScript parameters: staticity = 50, ease = 20
    const float staticity = 50.0f;
    const float ease = 20.0f;

    // Mouse position relativ zur Fenstermitte (wie in TypeScript)
    Vector2 mouse = GetMousePosition();
    float mx = mouse.x - (width / 2.0f);   // mouse.x - w/2
    float my = mouse.y - (height / 2.0f);  // mouse.y - h/2

    for (int i = 0; i < COUNT_CIRCLE_POINTS; ++i) {
      // 1. Konstante Drift (wie TypeScript: circle.x += circle.dx)
      circle_points[i].x += circle_points[i].dx;
      circle_points[i].y += circle_points[i].dy;

      // 2. Mouse influence mit magnetism (wie TypeScript)
      // translateX += (mouse.x / (staticity / magnetism) - translateX) / ease
      float mouse_factor_x = mx / (staticity / circle_points[i].magnetism);
      float mouse_factor_y = my / (staticity / circle_points[i].magnetism);

      circle_points[i].translateX += (mouse_factor_x - circle_points[i].translateX) / ease;
      circle_points[i].translateY += (mouse_factor_y - circle_points[i].translateY) / ease;

      // 3. Check if particle is out of bounds (wie TypeScript)
      float final_x = circle_points[i].x + circle_points[i].translateX;
      float final_y = circle_points[i].y + circle_points[i].translateY;
      float radius = static_cast<float>(circle_points[i].radius);

      if (final_x < -radius || final_x > width + radius ||
          final_y < -radius || final_y > height + radius) {
        // Reinitialize particle (wie TypeScript: neue Position und Werte)
        circle_points[i].x = static_cast<float>( GetRandomValue(0, width) );
        circle_points[i].y = static_cast<float>( GetRandomValue(0, height) );
        circle_points[i].translateX = 0.0f;
        circle_points[i].translateY = 0.0f;
        circle_points[i].dx = (GetRandomValue(0, 100) / 100.0f - 0.5f) * 0.2f;
        circle_points[i].dy = (GetRandomValue(0, 100) / 100.0f - 0.5f) * 0.2f;
        circle_points[i].magnetism = 0.1f + (GetRandomValue(0, 400) / 100.0f);
      }
    }
  }

  #ifndef PLATFORM_WEB

void print_usage( FILE *stream = stdout ) {
    std::fprintf( stream, "Usage: %s [Options]\n", flag_program_name( ) );
    print_app_info( stdout );
    std::printf( "\nOptions:\n" );
    flag_print_options( stdout );
  }

  #endif


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
    #ifdef PLATFORM_WEB
    bool     wallpaper_mode = false;  // NEW: Wallpaper mode flag
    #endif
    char    *title          = ( char * ) CBPS::DEFAULT_TITLE;
    char    *sub_title      = ( char * ) CBPS::DEFAULT_SUB_TITLE;
    uint64_t window_width   = CBPS::DEFAULT_WINDOW_WIDTH;
    uint64_t window_height  = CBPS::DEFAULT_WINDOW_HEIGHT;

    #ifndef PLATFORM_WEB
    flag_bool_var( &show_version, "-version", false, "Show version information" );
    flag_bool_var( &show_V_version, "v", false, "Show version information" );
    flag_bool_var( &show_help, "-help", false, "Show help information" );
    flag_bool_var( &show_h_help, "h", false, "Show help information" );
    flag_str_var( &title, "title", CBPS::DEFAULT_TITLE, "Set the window title" );
    flag_str_var( &sub_title, "subtitle", CBPS::DEFAULT_SUB_TITLE, "Set the window subtitle" );
    flag_uint64_var( &window_width, "width", CBPS::DEFAULT_WINDOW_WIDTH, "Set the window width" );
    flag_uint64_var( &window_height, "height", CBPS::DEFAULT_WINDOW_HEIGHT, "Set the window height" );
    // flag_bool_var( &wallpaper_mode, "wallpaper", false, "Run as desktop wallpaper" ); // Removed as per user request

    flag_parse( argc, argv );

    if ( show_version || show_V_version ) {
      printf( "Version: %s\n", CBPS::APP_VERSION );
      return 0;
    }

    if ( show_help || show_h_help ) {
      flag_print_options( stdout );
      return 0;
    }
    #endif

    // Seed einmalig hier, bevor Punkte initialisiert werden
    {
      auto now = std::chrono::system_clock::now();
      auto seed = static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
      SetRandomSeed(seed);
    }

    CBPS::init_circle_points( window_width, window_height );

    // Initialize raylib window
    // For wallpaper mode: no decorations, not maximized (will be positioned/sized later)
    SetConfigFlags( FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_ALWAYS_RUN | FLAG_WINDOW_UNFOCUSED | FLAG_WINDOW_UNDECORATED );
    InitWindow( window_width, window_height, CBPS::APP_NAME );

    // Set window as desktop wallpaper (platform-specific)
    #ifndef PLATFORM_WEB
    #if defined(__APPLE__)
        // macOS: Get native window handle from raylib
        void* nativeWindow = GetWindowHandle();

        // Get ACTUAL monitor dimensions (not window dimensions)
        int currentMonitor = GetCurrentMonitor();
        int screenWidth = GetMonitorWidth(currentMonitor);
        int screenHeight = GetMonitorHeight(currentMonitor);

        // macOS Menubar height is dynamic (especially with Notch on newer Macs)
        int menuBarHeight = DesktopWallpaper::GetMenuBarHeight();
        int windowY = menuBarHeight;
        int windowHeight = screenHeight - menuBarHeight;

        // Set window to full screen size (accounting for menubar) and position below menubar
        SetWindowPosition(0, windowY);
        SetWindowSize(screenWidth, windowHeight);

        // Now set as wallpaper
        DesktopWallpaper::SetAsWallpaper(nativeWindow);

        printf("macOS: Window positioned at 0,%d with size %dx%d (below menubar)\n",
               windowY, screenWidth, windowHeight);
    #elif defined(_WIN32)
        // Windows: Get HWND
        HWND hwnd = GetWindowHandle();
        DesktopWallpaper::SetAsWallpaper(hwnd);
    #elif defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
        // Linux/BSD: Get X11 Display and Window
        // Note: Raylib uses GLFW which uses X11 - we need to get those handles
        // This requires accessing GLFW internals or X11 directly
        // For now, commented out - requires additional GLFW/X11 integration
        // Display* display = glfwGetX11Display();
        // Window window = glfwGetX11Window(glfwGetCurrentContext());
        // DesktopWallpaper::SetAsWallpaper(display, window);
    #endif
    #endif

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

	// Load shaders for performance
	Shader particleShader = LoadShader("./assets/shaders/particle.vs", "./assets/shaders/particle.fs");
	Shader gradientShader = LoadShader("./assets/shaders/gradient.vs", "./assets/shaders/gradient.fs");

	if (particleShader.id == 0) {
		fprintf(stderr, "Warning: Failed to load particle shader, using default rendering\n");
	}
	if (gradientShader.id == 0) {
		fprintf(stderr, "Warning: Failed to load gradient shader, using default rendering\n");
	}

	// Get shader uniform locations
	int gradientColorALoc = GetShaderLocation(gradientShader, "colorA");
	int gradientColorBLoc = GetShaderLocation(gradientShader, "colorB");

    while ( !WindowShouldClose( ) ) {
      if ( IsWindowResized( ) ) {
        window_width = GetRenderWidth( );
        window_height = GetRenderHeight( );
        CBPS::init_circle_points( window_width, window_height );
      }

      // Update particle positions BEFORE drawing
      CBPS::update_circle_points( window_width, window_height );

      BeginDrawing( );
      ClearBackground( Nord::index_to_Color( Nord::NORD0 ) );
      CBPS::draw_circle_points( particleShader.id != 0 ? &particleShader : nullptr );

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

      // Draw top line as A -> B -> A using shader if available
      if (gradientShader.id != 0) {
        // Set shader uniform values
        float colorAFloat[4] = {colA.r/255.0f, colA.g/255.0f, colA.b/255.0f, colA.a/255.0f};
        float colorBFloat[4] = {colB.r/255.0f, colB.g/255.0f, colB.b/255.0f, colB.a/255.0f};
        SetShaderValue(gradientShader, gradientColorALoc, colorAFloat, SHADER_UNIFORM_VEC4);
        SetShaderValue(gradientShader, gradientColorBLoc, colorBFloat, SHADER_UNIFORM_VEC4);

        BeginShaderMode(gradientShader);
        // Draw top line
        DrawRectangle((int)lineLeft, (int)(titleY - 28.0f), (int)lineWidth, (int)lineThickness, WHITE);
        // Draw bottom line
        DrawRectangle((int)lineLeft, (int)(titleY + titleMeasure.y + 8.0f), (int)lineWidth, (int)lineThickness, WHITE);
        EndShaderMode();
      } else {
        // Fallback: Draw top line as A -> B -> A by drawing two halves
        int halfWidth = (int)(lineWidth * 0.5f);
        int leftX = (int)lineLeft;
        DrawRectangleGradientH(leftX, (int)(titleY - 28.0f), halfWidth, (int)lineThickness, colA, colB);
        DrawRectangleGradientH(leftX + halfWidth, (int)(titleY - 28.0f), (int)lineWidth - halfWidth, (int)lineThickness, colB, colA);
        // Draw bottom line as A -> B -> A
        DrawRectangleGradientH(leftX, (int)(titleY + titleMeasure.y + 8.0f), halfWidth, (int)lineThickness, colA, colB);
        DrawRectangleGradientH(leftX + halfWidth, (int)(titleY + titleMeasure.y + 8.0f), (int)lineWidth - halfWidth, (int)lineThickness, colB, colA);
      }

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
    }

    // Free loaded fonts and textures
    UnloadFont( titleFont );
    UnloadFont( subFont );
    if ( cbpsLoaded ) UnloadTexture( cbpsTex );
    if ( mahdLoaded ) UnloadTexture( mahdTex );
    if ( bdLoaded ) UnloadTexture( bdTex );
    if ( knudLoaded ) UnloadTexture( knudTex );
    if ( fabelLoaded ) UnloadTexture( fabelTex );

    // Unload shaders
    if (particleShader.id != 0) UnloadShader(particleShader);
    if (gradientShader.id != 0) UnloadShader(gradientShader);

    CloseWindow( );

    return 0;
  }
