#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// Texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

// Highlight controls
uniform vec3 highlightColor; // Color of the highlight
uniform bool isHighlighted;  // Whether the cube is being hovered on

void main()
{
    // Base color: mix the two textures
    vec4 baseColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);

    // If highlighted, blend the base color with the highlight color
    if (isHighlighted) {
        vec4 highlight = vec4(highlightColor, 1.0);
        FragColor = mix(baseColor, highlight, 0.5); // Blend 50% highlight
    } else {
        FragColor = baseColor; // Normal rendering
    }
}