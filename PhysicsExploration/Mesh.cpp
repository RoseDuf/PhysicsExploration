#include "Mesh.h"

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, vector<Material> materials)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    this->materials = materials;

    setupMesh();
}

Mesh::~Mesh()
{}

void Mesh::setupMesh()
{
	// VAOs makes switching between different vertex data and attribute configurations as easy as binding a different VAO.
	glGenVertexArrays(1, &VAO);
	// VBO store a large number of vertices in the GPU's memory.
	glGenBuffers(1, &VBO); // Buffer ID.
	// EBOs are used to solve the problem where vertices overlap
	glGenBuffers(1, &EBO);


	glBindVertexArray(VAO); //--------------
	// From this point on we should bind/configure the corresponding VBO(s) and attribute pointer(s) and then unbind the VAO for later use. 

	// 0. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // Buffer type for vertices
	// OpenGL allows us to bind to several buffers at once as long as they have a different buffer type.
	// From this point on, any buffer calls we make will be used to configure the currently bound buffer, which is vertices_VBO
	// Copy user-defined data into the currently bound buffer:
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices.front(), GL_STATIC_DRAW);
	// As of now we stored the vertex data within memory on the graphics card as managed by a vertex buffer object named vertices_VBO.

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices.front(), GL_STATIC_DRAW);

	// 1. then set the vertex attributes pointers
	// We have to specify how OpenGL should interpret the vertex data before rendering:
	// Remember that we specified the location of the position vertex attribute in the vertex shader with layout (location = 0)
	// Enable the vertex attribute, giving the vertex attribute location as its argument
	glEnableVertexAttribArray(0); // vertex positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1); //vertex normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

	//// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	glBindVertexArray(0); //--------------
	// Binding the appropriate buffer objectsand configuring all vertex attributes for each of those objects quickly becomes a cumbersome process.
	// What if there was some way we could store all these state configurations into an objectand simply bind this object to restore its state ?
	// Answer: VAO
}

void Mesh::Draw(Shader& shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;

	if (textures.size() > 0)
	{
		shader.setBool("hasTexture", true);

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
			// retrieve texture number (the N in diffuse_textureN)
			string number;
			string name = textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);
			else if (name == "texture_normal")
				number = std::to_string(normalNr++); // transfer unsigned int to stream
			else if (name == "texture_height")
				number = std::to_string(heightNr++); // transfer unsigned int to stream

			// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
			std::stringstream ss;
			ss << "materials[0]." << name /*<< number*/;
			std::string s = ss.str();

			shader.setInt((s).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
	}
	else
	{
		shader.setBool("hasTexture", false);
	}

	for (unsigned int j = 0; j < materials.size(); j++)
	{
		// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
		std::stringstream ss1, ss2, ss3, ss4;
		ss1 << "materials[" << j << "]." << "color_ambient";
		ss2 << "materials[" << j << "]." << "color_diffuse";
		ss3 << "materials[" << j << "]." << "color_specular";
		ss4 << "materials[" << j << "]." << "shininess";
		/*s3 << "materials[" << j << "]." << "texture_normal" << number;
		s4 << "materials[" << j << "]." << "texture_height" << number;*/
		std::string s1 = ss1.str();
		std::string s2 = ss2.str();
		std::string s3 = ss3.str();
		std::string s4 = ss4.str();

		shader.setVector((s1).c_str(), materials[j].Ambient);
		shader.setVector((s2).c_str(), materials[j].Diffuse);
		shader.setVector((s3).c_str(), materials[j].Specular);
		shader.setFloat((s4).c_str(), materials[j].Shininess);
	}

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
}