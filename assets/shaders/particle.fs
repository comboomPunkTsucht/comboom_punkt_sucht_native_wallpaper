#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Output fragment color
out vec4 finalColor;

void main()
{
    // Calculate distance from center
    vec2 center = vec2(0.5, 0.5);
    float dist = distance(fragTexCoord, center) * 2.0;

    // Create smooth circle with anti-aliasing
    float alpha = 1.0 - smoothstep(0.9, 1.0, dist);

    // Apply color with calculated alpha
    finalColor = vec4(fragColor.rgb, fragColor.a * alpha);
}
