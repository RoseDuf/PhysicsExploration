#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 lightSpace_matrix;
uniform mat4 model_matrix;

//We want to use a simple shader that only transforms the vertices to light space and not much more
void main() 
{
	gl_Position = lightSpace_matrix * model_matrix * vec4(aPos, 1.0);
}