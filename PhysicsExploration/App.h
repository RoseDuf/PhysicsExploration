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
	// Window dimensions
	const GLuint WIDTH = 800;
	const GLuint HEIGHT = 600;
	GLFWwindow* window;

	//camera settings
	Camera camera;

	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame

	//model settings
	glm::mat4 model = glm::mat4(1.0f); //to apply scalor and rotational transformations
	glm::vec3 modl_move = glm::vec3(0, 0, 0); //to apply translational transformations

	//color settings
	bool hasDirLight = true;
	bool hasPointLight = false;
	bool hasSpotLight = false;

	glm::vec4 lightPos;
	glm::vec4 lightDir;
	glm::vec3 pointLightPositions;

	bool firstMouse = true;
	float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
	float pitch = 0.0f;
	float lastX = WIDTH / 2.0;
	float lastY = HEIGHT / 2.0;
	float fov = 45.0f;
};

#endif