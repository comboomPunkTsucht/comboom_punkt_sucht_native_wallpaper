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
    NORD0 = 0x2E3440,
    NORD1 = 0x3B4252,
    NORD2 = 0x434C5E,
    NORD3 = 0x4C566A,
    NORD4 = 0xD8DEE9,
    NORD5 = 0xE5E9F0,
    NORD6 = 0xECEFF4,
    NORD7 = 0x8FBCBB,
    NORD8 = 0x88C0D0,
    NORD9 = 0x81A1C1,
    NORD10 = 0x5E81AC,
    NORD11 = 0xBF616A,
    NORD12 = 0xD08770,
    NORD13 = 0xEBCB8B,
    NORD14 = 0xA3BE8C,
    NORD15 = 0xB48EAD,
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
