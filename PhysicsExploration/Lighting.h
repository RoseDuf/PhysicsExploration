#ifndef LIGHTING_H
#define LIGHTING_H

#include "includes.h"
#include "Mesh.h"

class Lighting
{
	public:
		Lighting();
		~Lighting();

	private:
		unsigned int VAO, VBO;

		void setupLighting();
};

#endif 
