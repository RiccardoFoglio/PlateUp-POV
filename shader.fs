#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// Texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;



void main()
{
    // Base color: mix the two textures
    vec4 baseColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
    FragColor = baseColor; // Normal rendering
}