#include "cbps_wallpaper_engine_core.h"
#if !defined (__wasm__)
  #include <stdio.h>   // Für printf
  #include <stdlib.h>  // Für malloc, free
  #include <string.h>  // Für strlen, strdup

  #define CBPS_WE_strdup strdup
  #define CBPS_WE_strlen strlen
  #define CBPS_WE_malloc malloc
  #define CBPS_WE_free free
  #define CBPS_WE_rand rand

void cbps_log( const char* message ) {
  printf( message );
}

void cbps_log_num( const char* message, float number ) {
  printf( message, number );
}
#else

static unsigned int r_seed = 12345;  // Kann von außen einmal gesetzt werden

int internal_rand( ) {
  r_seed = r_seed * 1103515245 + 12345;
  return ( unsigned int ) ( r_seed / 65536 ) % 32768;
}

extern unsigned char __heap_base;
static unsigned int  heap_ptr = ( unsigned int ) &__heap_base;

void*                internal_malloc( unsigned int size ) {
  void* ptr = ( void* ) ( unsigned long ) heap_ptr;
  heap_ptr  = ( heap_ptr + size + 7 ) & ~7;  // 8-Byte Alignment
  return ptr;
}

// Free macht in WASM bei dir aktuell eh nichts, da beim Reload der Speichers
// komplett verworfen wird
void internal_free( void* ptr ) { }

// --- Interne Hilfsfunktionen (Ersatz für <string.h>) ---
static unsigned int internal_strlen( const char* str ) {
  unsigned int len = 0;
  while ( str && str[ len ] != '\0' ) {
    len++;
  }
  return len;
}

static char* internal_strdup( const char* str ) {
  if ( !str ) {
    return 0;  // NULL
  }
  unsigned int len  = internal_strlen( str );
  char*        copy = ( char* ) internal_malloc( len + 1 );  // +1 für das Null-Byte
  if ( copy ) {
    for ( unsigned int i = 0; i <= len; i++ ) {
      copy[ i ] = str[ i ];
    }
  }
  return copy;
}

extern void js_log( const char* message );
extern void js_log_num( const char* message, float number );

// 2. Die Funktionen, die du in deinem C-Code aufrufen kannst
void cbps_log( const char* message ) {
  js_log( message );
}

void cbps_log_num( const char* message, float number ) {
  js_log_num( message, number );
}

#define CBPS_WE_strdup internal_strdup
#define CBPS_WE_strlen internal_strlen
#define CBPS_WE_malloc internal_malloc
#define CBPS_WE_free internal_free
#define CBPS_WE_rand internal_rand

#endif

// --- API Implementierung ---

CBPSWallpaperEngine* cbps_engine_create(
  unsigned int screen_width, unsigned int screen_height,
  unsigned int particle_count, const char* h1, const char* h2) {

  CBPSWallpaperEngine* engine =
    ( CBPSWallpaperEngine* ) CBPS_WE_malloc( sizeof( CBPSWallpaperEngine ) );
  if ( !engine ) {
    return 0;
  }

  engine->screen_width   = screen_width;
  engine->screen_height  = screen_height;
  engine->particle_count = particle_count;

  engine->particles =
    ( CBPSParticle* ) CBPS_WE_malloc( sizeof( CBPSParticle ) * engine->particle_count );

  // Eigene String-Kopie-Funktion nutzen
  engine->h1 = CBPS_WE_strdup( h1 );
  engine->h2 = CBPS_WE_strdup( h2 );

  for ( int i = 0; i < engine->particle_count; i++ ) {
    // Position: irgendwo auf der Canvas
    engine->particles[i].x = (float)( CBPS_WE_rand( ) % screen_width );
    engine->particles[i].y = (float)( CBPS_WE_rand( ) % screen_height );

    // Mouse-Offset startet bei 0
    engine->particles[i].translateX = 0.0f;
    engine->particles[i].translateY = 0.0f;

    // Konstante Drift: (random - 0.5) * 0.2 → -0.1 bis +0.1
    engine->particles[i].dx = ((CBPS_WE_rand( ) % 100) / 100.0f - 0.5f) * 0.2f;
    engine->particles[i].dy = ((CBPS_WE_rand( ) % 100) / 100.0f - 0.5f) * 0.2f;

    // Magnetism: 0.1 + random * 4.0 → 0.1 bis 4.1
    engine->particles[i].magnetism = 0.1f + ((CBPS_WE_rand( ) % 400) / 100.0f);

    // Größe: 5 bis 8 (größer für bessere Sichtbarkeit)
    engine->particles[i].radius = (CBPS_WE_rand( ) % 3) + 3;

    // Farbe: Nord11 (rot) oder Nord14 (grün)
    engine->particles[i].color = (CBPS_WE_rand( ) % 2 == 0)
        ? CBPSRED
        : CBPSGREEN;

    // Alpha: 150 bis 230 (0.6 bis 0.9 in 0-255) - deutlich sichtbarer
    engine->particles[i].color.a = (unsigned char)( (CBPS_WE_rand( ) % 81) + 150 );
  }

  engine->background_color = CBPSBACKGROUND;
  engine->foreground_color = CBPSFOREGROUND;

  cbps_log( "Engine created.\n" );
  return engine;
}

void cbps_engine_update(
  CBPSWallpaperEngine* engine, float delta_time, const char* h1, const char* h2,
  unsigned int screen_width, unsigned int screen_height, int mouseX, int mouseY) {
  if ( !engine ) {
    return;
  }

  // Nutzen des injizierten 'free'
  if ( engine->h1 ) {
    CBPS_WE_free( engine->h1 );
  }
  if ( engine->h2 ) {
    CBPS_WE_free( engine->h2 );

  }

  const float staticity = 50.0f;
  const float ease = 20.0f;

  // Nutzen des injizierten 'malloc' über unsere Hilfsfunktion
  engine->h1            = CBPS_WE_strdup( h1 );
  engine->h2            = CBPS_WE_strdup( h2 );

  engine->screen_width  = screen_width;
  engine->screen_height = screen_height;

  float mx = mouseX - (screen_width / 2.0f);   // mouse.x - w/2
  float my = mouseY - (screen_height / 2.0f);

    for (int i = 0; i < engine->particle_count; ++i) {
      // 1. Konstante Drift (wie TypeScript: circle.x += circle.dx)
      engine->particles[i].x += engine->particles[i].dx;
      engine->particles[i].y += engine->particles[i].dy;

      // 2. Mouse influence mit magnetism (wie TypeScript)
      // translateX += (mouse.x / (staticity / magnetism) - translateX) / ease
      float mouse_factor_x = mx / (staticity / engine->particles[i].magnetism);
      float mouse_factor_y = my / (staticity / engine->particles[i].magnetism);

      engine->particles[i].translateX += (mouse_factor_x - engine->particles[i].translateX) / ease;
      engine->particles[i].translateY += (mouse_factor_y - engine->particles[i].translateY) / ease;

      // 3. Check if particle is out of bounds (wie TypeScript)
      float final_x = engine->particles[i].x + engine->particles[i].translateX;
      float final_y = engine->particles[i].y + engine->particles[i].translateY;


      float radius = (float)(engine->particles[i].radius);



      if (final_x < -radius || final_x > screen_width + radius ||
          final_y < -radius || final_y > screen_height + radius) {
        // Reinitialize particle (wie TypeScript: neue Position und Werte)
        engine->particles[i].x = (float)(CBPS_WE_rand( ) % screen_width);
        engine->particles[i].y = (float)(CBPS_WE_rand( ) % screen_height);

        engine->particles[i].translateX = 0.0f;
        engine->particles[i].translateY = 0.0f;


        engine->particles[i].dx = (float)((CBPS_WE_rand( ) % 100) / 100.0f - 0.5f) * 0.2f;
        engine->particles[i].dy = (float)((CBPS_WE_rand( ) % 100) / 100.0f - 0.5f) * 0.2f;

        engine->particles[i].magnetism = 0.1f + (float)((CBPS_WE_rand( ) % 400) / 100.0f);
      }
    }
}

const CBPSParticle* cbps_engine_get_particles(
  CBPSWallpaperEngine* engine, unsigned int* out_particle_count ) {
  if ( !engine ) {
    return 0;  // NULL
  }
  *out_particle_count = engine->particle_count;
  cbps_log_num( "Get Particles: %d\n", engine->particle_count );
  return engine->particles;
}

void cbps_engine_destroy( CBPSWallpaperEngine* engine) {
  if ( engine ) {
    // Wir nutzen die im Struct gespeicherte Host-Free-Funktion!
    if ( engine->h1 ) {
      CBPS_WE_free( engine->h1 );
    }
    if ( engine->h2 ) {
      CBPS_WE_free( engine->h2 );
    }
    if ( engine->particles ) {
      CBPS_WE_free( engine->particles );
    }

    CBPS_WE_free( engine );

    cbps_log( "Engine destroyed.\n" );
  }
}
