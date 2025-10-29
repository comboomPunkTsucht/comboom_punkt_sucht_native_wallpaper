// Copyright (c) 2025 mcpeaps_HD
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#include <cstdio>
#include <cstring>
#include <cstdint>

#ifndef NORD_COLORS
#define NORD_COLORS

namespace Nord {


  typedef enum {
    NORD0 = 0x2E3440ff,
    NORD1 = 0x3B4252ff,
    NORD2 = 0x434C5Eff,
    NORD3 = 0x4C566Aff,
    NORD4 = 0xD8DEE9ff,
    NORD5 = 0xE5E9F0ff,
    NORD6 = 0xECEFF4ff,
    NORD7 = 0x8FBCBBff,
    NORD8 = 0x88C0D0ff,
    NORD9 = 0x81A1C1ff,
    NORD10 = 0x5E81ACff,
    NORD11 = 0xBF616Aff,
    NORD12 = 0xD08770ff,
    NORD13 = 0xEBCB8Bff,
    NORD14 = 0xA3BE8Cff,
    NORD15 = 0xB48EADff,
  } NordColor;

  #ifdef NORD_COLORS_IMPLEMENTATION
  uint32_t index_to_uint32( NordColor color ) {
    return static_cast< uint32_t >( color );
  }

  char const *index_to_hexstr( NordColor color ) {
    static char buffer[8];
    std::snprintf( buffer, sizeof(buffer), "#%06X", index_to_uint32( color ) );
    return buffer;
  }
  #endif // NORD_COLORS_IMPLEMENTATION
} // namespace Nord

#endif // !NORD_COLORS
