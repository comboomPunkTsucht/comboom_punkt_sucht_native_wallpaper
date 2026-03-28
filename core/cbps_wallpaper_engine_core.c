#include "cbps_wallpaper_engine_core.h"

// --- Interne Hilfsfunktionen (Ersatz für <string.h>) ---
static unsigned int internal_strlen( const char* str ) {
  unsigned int len = 0;
  while ( str && str[ len ] != '\0' ) {
    len++;
  }
  return len;
}

static char* internal_strdup( const char* str, cbps_malloc_t m_fn ) {
  if ( !str || !m_fn ) {
    return 0;  // NULL
  }
  unsigned int len  = internal_strlen( str );
  char*        copy = ( char* ) m_fn( len + 1 );  // +1 für das Null-Byte
  if ( copy ) {
    for ( unsigned int i = 0; i <= len; i++ ) {
      copy[ i ] = str[ i ];
    }
  }
  return copy;
}

// --- API Implementierung ---

CBPSWallpaperEngine* cbps_engine_create(
  unsigned int screen_width, unsigned int screen_height,
  unsigned int particle_count, const char* h1, const char* h2,
  cbps_malloc_t m_fn, cbps_free_t f_fn, cbps_rand_t r_fn ) {
  // Sicherheitscheck: Ohne Memory-Allocator können wir nicht arbeiten
  if ( !m_fn || !f_fn || !r_fn ) {
    return 0;
  }

  CBPSWallpaperEngine* engine =
    ( CBPSWallpaperEngine* ) m_fn( sizeof( CBPSWallpaperEngine ) );
  if ( !engine ) {
    return 0;
  }

  engine->screen_width   = screen_width;
  engine->screen_height  = screen_height;
  engine->particle_count = particle_count;

  engine->particles =
    ( CBPSParticle* ) m_fn( sizeof( CBPSParticle ) * engine->particle_count );

  // Eigene String-Kopie-Funktion nutzen
  engine->h1 = internal_strdup( h1, m_fn );
  engine->h2 = internal_strdup( h2, m_fn );

  for ( int i = 0; i < engine->particle_count; i++ ) {
    // Position: irgendwo auf der Canvas
    engine->particles[i].x = (float)( r_fn( ) % screen_width );
    engine->particles[i].y = (float)( r_fn( ) % screen_height );

    // Mouse-Offset startet bei 0
    engine->particles[i].translateX = 0.0f;
    engine->particles[i].translateY = 0.0f;

    // Konstante Drift: (random - 0.5) * 0.2 → -0.1 bis +0.1
    engine->particles[i].dx = ((r_fn( ) % 100) / 100.0f - 0.5f) * 0.2f;
    engine->particles[i].dy = ((r_fn( ) % 100) / 100.0f - 0.5f) * 0.2f;

    // Magnetism: 0.1 + random * 4.0 → 0.1 bis 4.1
    engine->particles[i].magnetism = 0.1f + ((r_fn( ) % 400) / 100.0f);

    // Größe: 5 bis 8 (größer für bessere Sichtbarkeit)
    engine->particles[i].radius = (r_fn( ) % 3) + 3;

    // Farbe: Nord11 (rot) oder Nord14 (grün)
    engine->particles[i].color = (r_fn( ) % 2 == 0)
        ? CBPSRED
        : CBPSGREEN;
    // Alpha: 150 bis 230 (0.6 bis 0.9 in 0-255) - deutlich sichtbarer
    engine->particles[i].color.a = (unsigned char)( (r_fn( ) % 81) + 150 );
  }

  engine->background_color = CBPSBACKGROUND;
  engine->foreground_color = CBPSFOREGROUND;
  return engine;
}

void cbps_engine_update(
  CBPSWallpaperEngine* engine, float delta_time, const char* h1, const char* h2,
  unsigned int screen_width, unsigned int screen_height, int mouseX, int mouseY, cbps_malloc_t m_fn, cbps_free_t f_fn, cbps_rand_t r_fn) {
  if ( !engine  || !m_fn || !f_fn ) {
    return;
  }

  // Nutzen des injizierten 'free'
  if ( engine->h1 ) {
    f_fn( engine->h1 );
  }
  if ( engine->h2 ) {
    f_fn( engine->h2 );

  }

  const float staticity = 50.0f;
  const float ease = 20.0f;

  // Nutzen des injizierten 'malloc' über unsere Hilfsfunktion
  engine->h1            = internal_strdup( h1, m_fn );
  engine->h2            = internal_strdup( h2, m_fn );
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
        engine->particles[i].x = (float)(r_fn( ) % screen_width);
        engine->particles[i].y = (float)(r_fn( ) % screen_height);
        engine->particles[i].translateX = 0.0f;
        engine->particles[i].translateY = 0.0f;
        engine->particles[i].dx = (float)((r_fn( ) % 100) / 100.0f - 0.5f) * 0.2f;
        engine->particles[i].dy = (float)((r_fn( ) % 100) / 100.0f - 0.5f) * 0.2f;
        engine->particles[i].magnetism = 0.1f + (float)((r_fn( ) % 400) / 100.0f);
      }
    }
}

const CBPSParticle* cbps_engine_get_particles(
  CBPSWallpaperEngine* engine, unsigned int* out_particle_count ) {
  if ( !engine ) {
    return 0;  // NULL
  }
  *out_particle_count = engine->particle_count;
  return engine->particles;
}

void cbps_engine_destroy( CBPSWallpaperEngine* engine, cbps_free_t f_fn ) {
  if ( engine && f_fn ) {
    // Wir nutzen die im Struct gespeicherte Host-Free-Funktion!
    if ( engine->h1 ) {
      f_fn( engine->h1 );
    }
    if ( engine->h2 ) {
      f_fn( engine->h2 );
    }
    if ( engine->particles ) {
      f_fn( engine->particles );
    }

    f_fn( engine );
  }
}
