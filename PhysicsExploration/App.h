#ifndef APP_H
#define APP_H

#include "includes.h"
#include "Model.h"
#include "Camera.h"
#include "Lighting.h"
#include "stb_image.h"

class App
{
public:
	App();
	~App();


private:
	void renderScene(const Shader& shader);
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod);
	void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void processInput(GLFWwindow* window);
	unsigned int loadTexture(const char* path);

private:
	//camera settings
	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame

	//model settings
	glm::mat4 model = glm::mat4(1.0f); //to apply scalor and rotational transformations
	glm::vec3 modl_move = glm::vec3(0, 0, 0); //to apply translational transformations

	//color settings
	bool hasDirLight = true;
	bool hasPointLight = false;
	bool hasSpotLight = false;
};

#endif