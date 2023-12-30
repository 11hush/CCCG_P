#version 430 core
// reference: https://learnopengl.com/Advanced-OpenGL/Cubemaps
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
}