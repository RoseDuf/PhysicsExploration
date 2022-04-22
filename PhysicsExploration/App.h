#ifndef APP_H
#define APP_H

#include "CallbackHandler.h"
#include "Model.h"
#include "Camera.h"
#include "Lighting.h"
#include "stb_image.h"
#include <functional>

class App : CallbackHandler
{
public:
	App();
	~App();

	void initialize();

	void renderScene(const Shader& shader);
	void processInput(GLFWwindow* window);
	unsigned int loadTexture(const char* path);
	void initializeShaders();
	void initializeModels();
	void configureDepthBuffer();
	void addLights();
	void firstPass();
	void secondPass();
	void runApp();

	virtual void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	virtual void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod);
	virtual void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
	virtual void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	inline GLFWwindow* GetWindow() { return _window; }


	// Window dimensions
	const GLuint WIDTH = 800;
	const GLuint HEIGHT = 600;
	GLFWwindow* _window;

private:
	//camera settings
	Camera* _camera;

	float _deltaTime = 0.0f;	// Time between current frame and last frame
	float _lastFrame = 0.0f; // Time of last frame

	//shader variables
	const unsigned int SHADOW_WIDTH = 1024;
	const unsigned int SHADOW_HEIGHT = 1024;
	GLuint _depthMap;
	GLuint _depthMapFBO;
	Shader* _shader;
	Shader* _lightShader;
	Shader* _depthShader;

	//model settings
	Model* _plane;
	Model* _object;
	glm::mat4 _model = glm::mat4(1.0f); //to apply scalor and rotational transformations
	glm::vec3 _modl_move = glm::vec3(0, 0, 0); //to apply translational transformations

	//color settings
	bool _hasDirLight = true;
	bool _hasPointLight = false;
	bool _hasSpotLight = false;

	glm::vec4 _lightPos;
	glm::vec4 _lightDir;
	vector<glm::vec3> _pointLightPositions;

	bool _firstMouse = true;
	float _yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
	float _pitch = 0.0f;
	float _lastX = WIDTH / 2.0;
	float _lastY = HEIGHT / 2.0;
	float _fov = 45.0f;

	glm::mat4 _lightSpaceMatrix;

};

#endif