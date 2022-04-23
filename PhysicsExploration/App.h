#ifndef APP_H
#define APP_H

#include "CallbackHandler.h"
#include "Model.h"
#include "Camera.h"
#include "Lighting.h"
#include "MenuGUI.h"
#include "stb_image.h"
#include <fstream>
#include <string>
#include <functional>

class App : CallbackHandler
{
public:
	App();
	~App();

	int initializeWindow();
	void initializeCallbacks();

	void renderScene(const Shader& shader);
	void processInput();
	unsigned int loadTexture(const char* path);
	void initializeShaders();
	void initializeModels();
	void configureDepthBuffer();
	void addLights();
	void procressFrame();
	void firstPass();
	void secondPass();
	int runApp();
	void imGUIContextIntialization();
	void renderingGUIMenuIfEnabled();
	virtual void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	virtual void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod);
	virtual void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
	virtual void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
private:
	// Window options
	const GLuint WIDTH = 800;
	const GLuint HEIGHT = 600;
	GLFWwindow* _window;

	//camera settings
	Camera* _camera;
	bool _firstMouse;
	float _yaw;
	float _pitch;
	float _lastX;
	float _lastY;
	float _fov;

	//engine clock time
	float _deltaTime;
	float _lastFrame;

	//shader variables
	const unsigned int SHADOW_WIDTH = 1024;
	const unsigned int SHADOW_HEIGHT = 1024;
	glm::mat4 _lightSpaceMatrix;
	GLuint _depthMap;
	GLuint _depthMapFBO;
	Shader* _shader;
	Shader* _lightShader;
	Shader* _depthShader;

	//model settings
	Model* _plane;
	Model* _object;

	//color settings
	bool _hasDirLight;
	bool _hasPointLight;
	bool _hasSpotLight;

	//light settings
	glm::vec4 _lightPos;
	glm::vec4 _lightDir;
	vector<glm::vec3> _pointLightPositions;

	//imGUI or Menu variables
	bool _isMenuEnabled;
	int _selectedItem;
	int _simulationItemCount;
	char* _items[];
};

#endif