
#version 330 core
 
// We declare all the input vertex attributes in the vertex shader with the in keyword
// We also specifically set the location of the input variable via layout
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

//When we're talking specifically about the vertex shader each input variable is also known as a vertex attribute. 
//There is a maximum number of vertex attributes we're allowed to declare limited by the hardware.
//OpenGL guarantees there are always at least 16 4 - component vertex attributes available, 
//but some hardware may allow for more which you can retrieve by querying GL_MAX_VERTEX_ATTRIBS

//First of all, uniforms are global.Global, meaning that a uniform variable is unique per shader program object, 
//and can be accessed from any shader at any stage in the shader program.Second, whatever you set the uniform value to, 
//uniforms will keep their values until they're either reset or updated.
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;
uniform mat4 lightSpace_matrix;

out VS_OUT
{
	vec3 fragment_position;
	vec3 normal;
	vec2 texture_coords;
	vec4 fragment_position_lightSpace;
} vs_out;

void main()
{
	//normal = mat3(transpose(inverse(model_matrix))) * normals;
	//fragment_position = vec3(mm * vec4(position, 1.0f));
	//fragment_position_lightspace = lm * vec4(position, 1.0f);

	// At the end of the main function, whatever we set gl_Position to will be used as the output of the vertex shader.
	
	vs_out.texture_coords = aTexCoord;
	vs_out.fragment_position = vec3(model_matrix * vec4(aPosition, 1.0));
	vs_out.normal = mat3(transpose(inverse(model_matrix))) * aNormal; //avoid doing inverse function
	vs_out.fragment_position_lightSpace = lightSpace_matrix * vec4(vs_out.fragment_position, 1.0);

	gl_Position = proj_matrix * view_matrix * vec4(vs_out.fragment_position, 1.0);


}