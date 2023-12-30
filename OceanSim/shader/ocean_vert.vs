// reference::https://learnopengl.com/PBR/Theory
#version 440 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aNormal;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

layout(binding = 0) uniform sampler2D in_displacement;
layout(binding = 1) uniform sampler2D in_normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	vec3 position = texture(in_displacement,aTexCoords).rgb;
	vec3 normal = texture(in_normal,aTexCoords).rgb;

    WorldPos = vec3(model * vec4(position, 1.0));
    Normal = mat3(transpose(inverse(model)))* normal;   

    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}