// Copyright (c) 2025 mcpeaps_HD
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#include <print>
#ifndef CBPS_CONSTANTS
#define CBPS_CONSTANTS
namespace CBPS {
  constexpr const char *APP_NAME        = "comboom.sucht Live Wallpaper";
  constexpr const char *APP_VERSION     = "1.0.0";
  constexpr const char *APP_DESCRIPTION = "A native live wallpaper application using Qt6 and C++23.";

  constexpr const char *APP_AUTHOR      = "mcpeaps_HD Github: mcpeapsUnterstrichHD<mcpeaps_HD@outlook.com>";
  constexpr const char *APP_LICENSE     = "MIT";

  constexpr const char *APP_WEBSITE     = "https://combpoompunktsucht.app";
  constexpr const char *APP_REPO        = "https://github.com/comboomPunkTsucht/comboom_punkt_sucht_native_wallpaper";

  constexpr const int MIN_WINDOW_WIDTH  = 1280;
  constexpr const int MIN_WINDOW_HEIGHT = 720;

  //DEFAULTS
  constexpr const int DEFAULT_WINDOW_WIDTH  = 1920;
  constexpr const int DEFAULT_WINDOW_HEIGHT = 1080;
  constexpr const char *DEFAULT_TITLE       = "mcpeaps_HD";
  constexpr const char *DEFAULT_SUB_TITLE   = "comboom.sucht";

  #ifdef CBPS_CONSTANTS_IMPLEMENTATION

  inline void print_app_version( FILE *stream = stdout ) {
    std::fprintf( stream, "%s v%s\n", APP_NAME, APP_VERSION );
  }

  inline void print_app_info( FILE *stream = stdout ) {
    print_app_version();
    std::fprintf( stream, "%s\n", APP_DESCRIPTION );
    std::fprintf( stream, "Author: %s\n", APP_AUTHOR );
    std::fprintf( stream, "License: %s\n", APP_LICENSE );
    std::fprintf( stream, "Website: %s\n", APP_WEBSITE );
    std::fprintf( stream, "Repository: %s\n", APP_REPO );
  }



  #endif // !CBPS_CONSTANTS_IMPLEMENTATION

} // namespace CBPS

#endif // !CBPS_CONSTANTS
