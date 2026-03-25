//
//  Particles.metal
//  comboom.sucht Live Wallpaper
//
//  Created by mcpeaps_HD on 25/03/2026.
//

#include <metal_stdlib>
using namespace metal;

  // Exakt deine neue Struktur!
struct Particle {
  float x;
  float y;
  float translateX;
  float translateY;
  float dx;
  float dy;
  float magnetism;
  int radius;
  uchar4 color;
};

struct VertexOutput {
  float4 position [[position]];
  float4 color;
  float pointSize [[point_size]];
};

vertex VertexOutput vertex_main(
                                uint vertexID [[vertex_id]],
                                constant Particle *particles [[buffer(0)]],
                                constant float2 *screenSize [[buffer(1)]])
{
  VertexOutput out;
  Particle p = particles[vertexID];

    // ECHTE Position berechnen (Basis + Mouse-Offset)
  float realX = p.x + p.translateX;
  float realY = p.y + p.translateY;

  float2 normalizedPos = (float2(realX, realY) / *screenSize) * 2.0 - 1.0;
  out.position = float4(normalizedPos.x, -normalizedPos.y, 0.0, 1.0);

  out.color = float4(p.color) / 255.0;

    // int radius in float umwandeln. (Multipliziert mit 2, da pointSize der Durchmesser ist)
  out.pointSize = float(p.radius) * 2.0;

  return out;
}

fragment float4 fragment_main(
                              VertexOutput in [[stage_in]],
                              float2 pointCoord [[point_coord]]
                              ) {
  float dist = distance(pointCoord, float2(0.5, 0.5));
  float alpha = 1.0 - smoothstep(0.45, 0.5, dist);

  if (alpha <= 0.0) {
    discard_fragment();
  }

  float4 finalColor = in.color;
  finalColor.a *= alpha;
  return finalColor;
}
