#version 450

// Input from vertex shader
layout(location = 0) in VS_OUT {
    vec4 color;
} fs_in;

// Output
layout(location = 0) out vec4 out_color;

void main() {
    // Create circular soft particle using point coordinates
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);

    // Soft circle with anti-aliasing
    float alpha = smoothstep(0.55, 0.45, dist);

    // Output color with alpha
    out_color = vec4(fs_in.color.rgb, fs_in.color.a * alpha);
}
