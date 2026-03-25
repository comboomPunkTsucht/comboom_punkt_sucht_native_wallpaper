#ifndef CBPS_WALLPAPER_ENGINE_CORE_H
#define CBPS_WALLPAPER_ENGINE_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

  // --- Eigene Typen für die Host-Funktionen (Dependency Injection) ---
  typedef void* ( *cbps_malloc_t )( unsigned int size );
  typedef void  ( *cbps_free_t )( void* ptr );
  typedef int   ( *cbps_rand_t )( void );

  typedef struct {
      unsigned char r, g, b, a;
  } CBPSColor;

#define CBPSRED        ( CBPSColor ){ 0xbf, 0x61, 0x6a, 0xff }
#define CBPSGREEN      ( CBPSColor ){ 0xa3, 0xbe, 0x8c, 0xff }
#define CBPSBACKGROUND ( CBPSColor ){ 0x2e, 0x34, 0x40, 0xff }
#define CBPSFOREGROUND ( CBPSColor ){ 0xec, 0xef, 0xf4, 0xff }

  typedef struct {
    float x;           // aktuelle Position x
    float y;           // aktuelle Position y
    float translateX;  // Mouse-Offset x (smooth)
    float translateY;  // Mouse-Offset y (smooth)
    float dx;          // konstante Drift-Geschwindigkeit x
    float dy;          // konstante Drift-Geschwindigkeit y
    float magnetism;   // 0.1 .. 4.1, Anziehung zur Maus
    int   radius;
    CBPSColor color;
  } CBPSParticle;

  typedef struct {
      CBPSParticle* particles;
      unsigned int  particle_count;
      unsigned int  screen_height;
      unsigned int  screen_width;
      CBPSColor     background_color;
      CBPSColor     foreground_color;
      char*         h1;
      char*         h2;
  } CBPSWallpaperEngine;

  // --- Die öffentliche C-API ---

  // Die Host-Plattform übergibt hier ihre eigenen alloc, free und rand
  // Funktionen
  CBPSWallpaperEngine* cbps_engine_create(
    unsigned int screen_width, unsigned int screen_height,
    unsigned int particle_count, const char* h1, const char* h2,
    cbps_malloc_t m_fn, cbps_free_t f_fn, cbps_rand_t r_fn );

  void cbps_engine_update(
    CBPSWallpaperEngine* engine, float delta_time, const char* h1,
    const char* h2, unsigned int screen_width, unsigned int screen_height, int mouseX, int mouseY, cbps_malloc_t m_fn, cbps_free_t f_fn, cbps_rand_t r_fn);

  const CBPSParticle* cbps_engine_get_particles(
    CBPSWallpaperEngine* engine, unsigned int* out_particle_count );

  void cbps_engine_destroy( CBPSWallpaperEngine* engine, cbps_free_t f_fn );

#ifdef __cplusplus
}
#endif

#endif  // CBPS_WALLPAPER_ENGINE_CORE_H
