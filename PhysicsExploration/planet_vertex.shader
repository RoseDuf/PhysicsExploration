#version 330				

layout(location = 0) in  vec3 pos;						

uniform mat4 model_matrix;

void main()	
{									
	gl_Position = model_matrix * vec4(0.4 * pos.x, 0.4 * pos.y, 0.4 * pos.z, 1.0);
}