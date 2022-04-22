#include "App.h"

App::App()
{
	setApp();

	_camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	_lightPos = glm::vec4(1.2f, 1.0f, 2.0f, 1.0f);
	_lightDir = glm::vec4(-0.2f, -1.0f, -0.3f, 0.0f);
	_pointLightPositions = {
						glm::vec3(0.7f,  0.2f,  2.0f),
						glm::vec3(2.3f, -3.3f, -4.0f),
						glm::vec3(-4.0f,  2.0f, -12.0f),
						glm::vec3(0.0f,  0.0f, -3.0f) };
	_firstMouse = true;
	_yaw = -90.0f;
	_pitch = 0.0f;
	_lastX = WIDTH / 2.0;
	_lastY = HEIGHT / 2.0;
	_fov = 45.0f;

	_hasDirLight = true;
	_hasPointLight = false;
	_hasSpotLight = false;

	_deltaTime = 0;
	_lastFrame = 0;
}

App::~App()
{
	delete _camera;
	delete _shader;
	delete _lightShader;
	delete _depthShader;
	delete _plane;
	delete _object;
}

int App::initializeWindow()
{
	std::cout << "Starting GLFW context, OpenGL 4.3" << std::endl;
	MenuGUI menuGui;
	menuGui.loadMenu();
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	//WINDOW
	_window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL_Project", nullptr, nullptr);

	if (nullptr == _window)
	{
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(_window);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;

	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}

void App::initializeCallbacks()
{
	glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback_dispatch);
	glfwSetKeyCallback(_window, key_callback_dispatch);
	glfwSetCursorPosCallback(_window, cursor_pos_callback_dispatch);
	glfwSetScrollCallback(_window, scroll_callback_dispatch);
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetMouseButtonCallback(window, mouse_button_callback);
	//glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);

	stbi_set_flip_vertically_on_load(true);
}

void App::initializeShaders()
{
	// 2. use our shader program when we want to render an object
	// Build and compile our shader program
	// Vertex shader.
	_shader = new Shader("../PhysicsExploration/vertex.shader", "../PhysicsExploration/fragment.shader");
	_lightShader = new Shader("../PhysicsExploration/light_vertex.shader", "../PhysicsExploration/light_fragment.shader");
	_depthShader = new Shader("../PhysicsExploration/shadow_vertex.shader", "../PhysicsExploration/shadow_fragment.shader");
}

void App::initializeModels()
{
	//Model object("../External Resources/Models/cat.obj");
	_plane = new Model("../External Resources/Models/plane.obj");
	_object = new Model("../External Resources/Models/backpack.obj");
}

void App::configureDepthBuffer()
{
	glEnable(GL_DEPTH_TEST);

	//DEPTH MAP - Configure the framebuffer
	//We create a 2D texture that we'll use as the framebuffer's depth buffer
	glGenTextures(1, &_depthMap);
	glBindTexture(GL_TEXTURE_2D, _depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//With the generated depth texture we can attach it as the framebuffer's depth buffer
	glGenFramebuffers(1, &_depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthMap, 0);
	//We only need the depth information when rendering the scene from the light's perspective so there is no need for a color buffer. 
	//A framebuffer object however is not complete without a color buffer so we need to explicitly tell OpenGL we're not going to render any color data. 
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void App::addLights()
{
	_shader->use();

	_shader->setInt("shadowMap", 0);

	_shader->setVector("dirLight.position", _lightPos);
	_shader->setVector("dirLight.direction", _lightDir);
	_shader->setVector("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	_shader->setVector("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
	_shader->setVector("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

	for (GLuint i = 0; i < (sizeof(_pointLightPositions) / sizeof(_pointLightPositions[0])) - 1; i++)
	{
		string number = to_string(i);

		_shader->setVector("pointLights[" + number + "].position", _pointLightPositions[i]);
		_shader->setVector("pointLights[" + number + "].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		_shader->setVector("pointLights[" + number + "].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		_shader->setVector("pointLights[" + number + "].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		_shader->setFloat("pointLights[" + number + "].constant", 1.0f);
		_shader->setFloat("pointLights[" + number + "].linearK", 0.09f);
		_shader->setFloat("pointLights[" + number + "].quadratic", 0.032f);
	}

	_shader->setVector("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
	_shader->setVector("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
	_shader->setVector("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	_shader->setFloat("spotLight.constant", 1.0f);
	_shader->setFloat("spotLight.linearK", 0.09f);
	_shader->setFloat("spotLight.quadratic", 0.032f);
	_shader->setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	_shader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
}

void App::firstPass()
{
	//First pass : render depth of scene to texture (from light's perspective)
	// --------------------------------------------------------------
	//Use an orthographic projection matrix for the light source where there is no perspective deform
	float near_plane = 1.0f, far_plane = 7.5f;
	glm::mat4 shadow_proj_matrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	//To transform each object so they're visible from the light's point of view we use lookAt for the view matrix
	glm::mat4 shadow_view_matrix = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//Combining these two gives us a light space transformation matrix that transforms each world-space vector into the space as visible from the light source
	_lightSpaceMatrix = shadow_proj_matrix * shadow_view_matrix;

	_depthShader->use();
	_depthShader->setMatrix4("lightSpace_matrix", _lightSpaceMatrix);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT); //If we forget to update the viewport parameters, the resulting depth map will be either incomplete or too small
	glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);

	// shader stuff vvvvvvvvvvv
	renderScene(*_depthShader);
	_object->Draw(*_depthShader);
	_plane->Draw(*_depthShader);
	// shader stuff ^^^^^^^^^^^^^^^

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void App::secondPass()
{
	//Second pass : render scene as normal with shadow mapping (using depth map)
	// --------------------------------------------------------------
	glViewport(0, 0, WIDTH, HEIGHT);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader stuff vvvvvvvvvvv
	_shader->use();

	_shader->setMatrix4("lightSpace_matrix", _lightSpaceMatrix);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _depthMap);

	renderScene(*_shader);
	_object->Draw(*_shader);
	_plane->Draw(*_shader);
	// shader stuff ^^^^^^^^^^^^^^^
}

void App::procressFrame()
{
	float currentFrame = glfwGetTime();
	_deltaTime = currentFrame - _lastFrame;
	_lastFrame = currentFrame;
}

int App::runApp()
{
	if (initializeWindow() != 0)
		return EXIT_FAILURE;

	initializeCallbacks();

	initializeShaders();
	initializeModels();
	configureDepthBuffer();
	addLights();

	while (!glfwWindowShouldClose(_window))
	{
		procressFrame();
		processInput();

		firstPass();
		secondPass();

		// Swap the screen buffers
		glfwSwapBuffers(_window);
		glfwPollEvents();
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
}

void App::renderScene(const Shader& shader)
{
	shader.setVector("spotLight.position", glm::vec4(_camera->GetPosition(), 1.0f));
	shader.setVector("spotLight.direction", _camera->GetFront());

	shader.setVector("view_position", _camera->GetPosition());

	glm::mat4 proj_matrix = glm::mat4(1.0f);
	proj_matrix = glm::perspective(glm::radians(_camera->GetZoom()), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);
	shader.setMatrix4("proj_matrix", proj_matrix);

	glm::mat4 view_matrix = _camera->GetViewMatrix();
	shader.setMatrix4("view_matrix", view_matrix);

	// render the loaded model
	glm::mat4 model_matrix = glm::mat4(1.0f);
	model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	model_matrix = glm::scale(model_matrix, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
	shader.setMatrix4("model_matrix", model_matrix);

	shader.setBool("hasDirLight", _hasDirLight);
	shader.setBool("hasPointLight", _hasPointLight);
	shader.setBool("hasSpotLight", _hasSpotLight);
}

void App::processInput()
{
	if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(_window, true);

	if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
		_camera->ProcessKeyboard(FORWARD, 0.01 * _deltaTime);
	if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
		_camera->ProcessKeyboard(BACKWARD, 0.01 * _deltaTime);
	if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
		_camera->ProcessKeyboard(LEFT, 0.01 * _deltaTime);
	if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
		_camera->ProcessKeyboard(RIGHT, 0.01 * _deltaTime);
	if (glfwGetKey(_window, GLFW_KEY_R) == GLFW_PRESS)
		_camera->ProcessKeyboard(UP, 0.01 * _deltaTime);
	if (glfwGetKey(_window, GLFW_KEY_F) == GLFW_PRESS)
		_camera->ProcessKeyboard(DOWN, 0.01 * _deltaTime);
}

unsigned int App::loadTexture(const char* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void App::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


void App::key_callback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
	//if you press the Esc key, the window will close
	if (key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window, GL_TRUE);

	//toggle the directional light on/off
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		_hasDirLight = !_hasDirLight;
	}

	//toggle the point light(s) on/off
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		_hasPointLight = !_hasPointLight;
	}

	//toggle the spot light on/off
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		_hasSpotLight = !_hasSpotLight;
	}
}

void App::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (_firstMouse)
	{
		_lastX = xpos;
		_lastY = ypos;
		_firstMouse = false;
	}

	float xoffset = xpos - _lastX;
	float yoffset = _lastY - ypos;
	_lastX = xpos;
	_lastY = ypos;

	_camera->ProcessMouseMovement(xoffset, yoffset);
}

void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	_camera->ProcessMouseScroll(yoffset);
}