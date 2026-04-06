#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 0) out vec4 outColor;

void main()
{
    // Create circular particle with soft edges
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);
    
    if (dist > 0.5) {
        discard;
    }
    
    // Soft glow at edges
    float alpha = 1.0 - smoothstep(0.4, 0.5, dist);
    outColor = vec4(fragColor.rgb, alpha * fragColor.a);
}
