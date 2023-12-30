#version 330 core
out vec4 FragColor;
uniform vec4 inColor;		// uniform inColor as input parameter

void main()
{
	FragColor = inColor;	// set the color
}