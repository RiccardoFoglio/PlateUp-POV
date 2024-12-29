#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;      // Model matrix for transforming the bounding box
uniform mat4 view;       // View matrix from the camera
uniform mat4 projection; // Projection matrix

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
