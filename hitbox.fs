#version 330 core
out vec4 FragColor;

uniform vec3 color; // Color of the wireframe (e.g., red for debugging)

void main()
{
    FragColor = vec4(color, 1.0);
}
