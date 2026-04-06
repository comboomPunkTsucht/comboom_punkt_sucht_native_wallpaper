#version 450

layout(binding = 0) readonly buffer ParticleBuffer {
    vec4 particles[];
};

layout(push_constant) uniform PushConstants {
    vec2 screenSize;
    vec2 mousePos;
} pc;

layout(location = 0) out vec4 fragColor;

void main()
{
    // Get particle data from SSBO
    vec4 particle = particles[gl_InstanceIndex];
    vec2 pos = particle.xy;

    // Apply mouse offset (optional)
    vec2 toMouse = pc.mousePos - pos;
    pos += normalize(toMouse) * 0.01;

    // Convert to NDC
    vec2 ndc = (pos / pc.screenSize) * 2.0 - 1.0;
    ndc.y = -ndc.y; // Flip Y for Vulkan

    gl_Position = vec4(ndc, 0.0, 1.0);
    gl_PointSize = 5.0;

    // Pass color
    fragColor = vec4(particle.zw, 1.0, 1.0);
}
