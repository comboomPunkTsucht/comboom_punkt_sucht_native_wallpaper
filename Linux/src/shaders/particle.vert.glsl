#version 450

// Input: Particle vertex data
layout(binding = 0, std430) readonly buffer ParticleBuffer {
    vec4 pos;        // x, y, translateX, translateY
    vec4 vel;        // dx, dy, magnetism, radius
    vec4 color;      // r, g, b, a (0-1 normalized)
} particles[];

// Output
layout(location = 0) out VS_OUT {
    vec4 color;
} vs_out;

// Push constants for screen dimensions
layout(push_constant) uniform PushConstants {
    vec2 screen_size;
} pc;

void main() {
    // Get particle data
    vec4 particle_pos = particles[gl_VertexIndex].pos;
    vec4 particle_vel = particles[gl_VertexIndex].vel;
    vec4 particle_color = particles[gl_VertexIndex].color;

    float x = particle_pos.x + particle_pos.z;  // x + translateX
    float y = particle_pos.y + particle_pos.w;  // y + translateY
    float radius = particle_vel.w;

    // Transform to clip space (-1 to 1)
    float clip_x = (x / pc.screen_size.x) * 2.0 - 1.0;
    float clip_y = 1.0 - (y / pc.screen_size.y) * 2.0;  // Flip Y for Vulkan NDC

    gl_Position = vec4(clip_x, clip_y, 0.0, 1.0);
    gl_PointSize = radius * 2.0;

    // Pass color to fragment shader
    vs_out.color = particle_color;
}
