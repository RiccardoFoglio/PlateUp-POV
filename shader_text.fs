#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{    
    float alpha = texture(text, TexCoords).r; // Fetch alpha from the texture
    if (alpha < 0.1) // Optional: discard near-transparent pixels
        discard;

    color = vec4(textColor, alpha); // Use the alpha value for blending
}
