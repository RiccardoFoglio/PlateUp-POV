#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;  // Fragment position in world space
out vec3 Normal;   // Normal vector in world space
out vec2 TexCoords; // Texture coordinates

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;    

    // Transform fragment position to world space
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Transform normal to world space (account for model scaling)
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // Calculate final vertex position
    gl_Position = projection * view * vec4(FragPos, 1.0);

    // gl_Position = projection * view * model * vec4(aPos, 1.0);
}
