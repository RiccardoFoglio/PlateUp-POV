#version 330 core
out vec4 FragColor;


in vec3 FragPos;  // From vertex shader
in vec3 Normal;   // From vertex shader
in vec2 TexCoords; // From vertex shader
//in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

uniform vec3 lightPos;      // Light position in world space
uniform vec3 viewPos;       // Camera/view position
uniform vec3 lightColor;    // Light color
uniform vec3 objectColor;   // Object's base color (optional if texture is used)

//uniform vec3 lightColor;
//uniform vec3 objectColor;

void main()
{    

    // Ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular lighting
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Combine results
    vec3 lighting = ambient + diffuse + specular;

    // Apply texture color and lighting
    vec3 textureColor = vec3(texture(texture_diffuse1, TexCoords));
    vec3 finalColor = lighting * textureColor; 

    FragColor = vec4(finalColor, 1.0);


    //FragColor = texture(texture_diffuse1, TexCoords);
    //FragColor = vec4(lightColor * objectColor, 1.0);

}