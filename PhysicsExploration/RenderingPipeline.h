#pragma once

#include "CallbackHandler.h"
#include "Model.h"
#include "Camera.h"
#include "Lighting.h"
#include "MenuGUI.h"
#include "stb_image.h"
#include "Shader.h"
#include <fstream>
#include <string>
#include <functional>
class RenderingPipeline : CallbackHandler
{
public:
	RenderingPipeline();
	~RenderingPipeline();
	int runAppTemplate() const;
	void processInput() const;
protected:
	int initializeWindow() const;
	void initializeCallbacks() const;
	virtual void initializeShaders() const = 0;
	virtual void initializeModels() const = 0;
	void static configureDepthBuffer();
	void addLights() const;
	void processFrame() const;
	void firstPass() const;
	void secondPass() const;
	virtual void renderScene(const Shader& shader) const = 0;
	virtual void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	virtual void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod);
	virtual void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
	virtual void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	// Window options
	const GLuint WIDTH = 800;
	const GLuint HEIGHT = 600;
	static GLFWwindow* _window;
	//camera settings
	Camera* _camera;
	bool _firstMouse;
	float _yaw;
	float _pitch;
	float _lastX;
	float _lastY;
	float _fov;

	//engine clock time
	static float _deltaTime;
	static float _lastFrame;

	//shader variables
	const static unsigned int SHADOW_WIDTH = 1024;
	const static unsigned int SHADOW_HEIGHT = 1024;
	static glm::mat4 _lightSpaceMatrix;
	static GLuint _depthMap;
	static GLuint _depthMapFBO;
	static Shader* _shader;
	static Shader* _lightShader;
	static Shader* _depthShader;

	//model settings
	static Model* _plane;
	static Model* _object;

	//color settings
	bool _hasDirLight;
	bool _hasPointLight;
	bool _hasSpotLight;

	//light settings
	glm::vec4 _lightPos;
	glm::vec4 _lightDir;
	vector<glm::vec3> _pointLightPositions;

	//imGUI or Menu variables
	static bool _isMenuEnabled;
	static int _selectedItem;
	int _simulationItemCount;

	void imGUIContextIntialization() const;
	static void renderGUIMenuIfEnabled();
	void imGUIEndContext() const;
};

