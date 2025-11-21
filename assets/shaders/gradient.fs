#version 330

// Input from vertex shader
in vec2 fragTexCoord;
in vec4 fragColor;

// Uniforms
uniform vec4 colorA;
uniform vec4 colorB;

// Output fragment color
out vec4 finalColor;

void main()
{
    // Horizontal gradient based on X coordinate
    float t = fragTexCoord.x;

    // Mirror gradient in the middle for A -> B -> A effect
    if (t > 0.5) {
        t = 1.0 - t;
    }
    t *= 2.0; // Scale back to 0-1

    // Mix colors
    finalColor = mix(colorA, colorB, t);
}
