//modified from http://learnopengl.com/

#include "stdafx.h"
#include "Model.h"
#include "Camera.h"
#include "Lighting.h"
#include "stb_image.h"
using namespace std;

int init();
void renderScene(const Shader& shader);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod);
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
GLFWwindow* window;

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

glm::vec4 lightPos(1.2f, 1.0f, 2.0f, 1.0f);
glm::vec4 lightDir(-0.2f, -1.0f, -0.3f, 0.0f);
glm::vec3 pointLightPositions[] = {
								glm::vec3(0.7f,  0.2f,  2.0f),
								glm::vec3(2.3f, -3.3f, -4.0f),
								glm::vec3(-4.0f,  2.0f, -12.0f),
								glm::vec3(0.0f,  0.0f, -3.0f) };


bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = WIDTH / 2.0;
float lastY = HEIGHT / 2.0;
float fov = 45.0f;

// The MAIN function, from here we start the application and run the game loop
int main()
{
	if (init() != 0)
		return EXIT_FAILURE;
	// Set the required callback functions

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetMouseButtonCallback(window, mouse_button_callback);
	//glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);

	stbi_set_flip_vertically_on_load(true);

	// 2. use our shader program when we want to render an object
	// Build and compile our shader program
	// Vertex shader
	Shader shader = Shader("../PhysicsExploration/vertex.shader", "../PhysicsExploration/fragment.shader");
	Shader light_shader = Shader("../PhysicsExploration/light_vertex.shader", "../PhysicsExploration/light_fragment.shader");
	Shader depth_shader = Shader("../PhysicsExploration/shadow_vertex.shader", "../PhysicsExploration/shadow_fragment.shader");

	//Model object("../External Resources/Models/cat.obj");
	Model plane("../External Resources/Models/plane.obj");
	Model object("../External Resources/Models/backpack.obj");

	glEnable(GL_DEPTH_TEST);

	//DEPTH MAP - Configure the framebuffer
	//We create a 2D texture that we'll use as the framebuffer's depth buffer
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//With the generated depth texture we can attach it as the framebuffer's depth buffer
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	//We only need the depth information when rendering the scene from the light's perspective so there is no need for a color buffer. 
	//A framebuffer object however is not complete without a color buffer so we need to explicitly tell OpenGL we're not going to render any color data. 
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	

	shader.use();

	shader.setInt("shadowMap", 0);
	
	shader.setVector("dirLight.position", lightPos);
	shader.setVector("dirLight.direction", lightDir);
	shader.setVector("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	shader.setVector("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
	shader.setVector("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

	for (GLuint i = 0; i < (sizeof(pointLightPositions)/sizeof(pointLightPositions[0])) - 1; i++)
	{
		string number = to_string(i);

		shader.setVector("pointLights[" + number + "].position", pointLightPositions[i]);
		shader.setVector("pointLights[" + number + "].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		shader.setVector("pointLights[" + number + "].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		shader.setVector("pointLights[" + number + "].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		shader.setFloat("pointLights[" + number + "].constant", 1.0f);
		shader.setFloat("pointLights[" + number + "].linearK", 0.09f);
		shader.setFloat("pointLights[" + number + "].quadratic", 0.032f);
	}
	
	shader.setVector("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
	shader.setVector("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setVector("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setFloat("spotLight.constant", 1.0f);
	shader.setFloat("spotLight.linearK", 0.09f);
	shader.setFloat("spotLight.quadratic", 0.032f);
	shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

	depth_shader.use();
	depth_shader.setInt("depthMap", 0);
	//shadow load shader
	//Shader shadow_shader = Shader("shadow_vertex.shader", "shadow_fragment.shader");
	//shadow_shader.use();

	//GLuint shadow_mm_loc = glGetUniformLocation(shadow_shader, "mm");
	//GLuint shadow_pvm_loc = glGetUniformLocation(shadow_shader, "pvm");


	// Game loop

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		////glm::mat4 rotator = glm::rotate(glm::mat4(1.0f), 1.57f, glm::vec3(0, 1, 0));
		//glm::mat4 translator = glm::translate(glm::mat4(1.0f), modl_move);
		////glm::mat4 scalor = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));
		//glm::mat4 modl_matrix = translator * model;

		//DEPTH MAP
		//First pass : render depth of scene to texture (from light's perspective)
		// --------------------------------------------------------------
		//Use an orthographic projection matrix for the light source where there is no perspective deform
		float near_plane = 1.0f, far_plane = 7.5f;
		glm::mat4 shadow_proj_matrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		//To transform each object so they're visible from the light's point of view we use lookAt for the view matrix
		glm::mat4 shadow_view_matrix = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//Combining these two gives us a light space transformation matrix that transforms each world-space vector into the space as visible from the light source
		glm::mat4 lightSpaceMatrix = shadow_proj_matrix * shadow_view_matrix;

		depth_shader.use();
		depth_shader.setMatrix4("lightSpace_matrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT); //If we forget to update the viewport parameters, the resulting depth map will be either incomplete or too small
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			glActiveTexture(GL_TEXTURE0);

			// shader stuff vvvvvvvvvvv
			renderScene(depth_shader);
			object.Draw(depth_shader);
			plane.Draw(depth_shader);
			// shader stuff ^^^^^^^^^^^^^^^

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		//Second pass : render scene as normal with shadow mapping (using depth map)
		// --------------------------------------------------------------
		glViewport(0, 0, WIDTH, HEIGHT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// shader stuff vvvvvvvvvvv
		shader.use();

		shader.setMatrix4("lightSpace_matrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		renderScene(shader);
		object.Draw(shader);
		plane.Draw(shader);
		// shader stuff ^^^^^^^^^^^^^^^

		// Swap the screen buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	/*glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);*/

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

int init() {
	std::cout << "Starting GLFW context, OpenGL 4.3" << std::endl;
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	//WINDOW
	window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL_Project", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


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

// renders the 3D scene
// --------------------
void renderScene(const Shader& shader)
{
	shader.setVector("spotLight.position", glm::vec4(camera.GetPosition(), 1.0f));
	shader.setVector("spotLight.direction", camera.GetFront());

	shader.setVector("view_position", camera.GetPosition());

	glm::mat4 proj_matrix = glm::mat4(1.0f);
	proj_matrix = glm::perspective(glm::radians(camera.GetZoom()), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);
	shader.setMatrix4("proj_matrix", proj_matrix);

	glm::mat4 view_matrix = camera.GetViewMatrix();
	shader.setMatrix4("view_matrix", view_matrix);

	// render the loaded model
	glm::mat4 model_matrix = glm::mat4(1.0f);
	model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	model_matrix = glm::scale(model_matrix, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
	shader.setMatrix4("model_matrix", model_matrix);

	shader.setBool("hasDirLight", hasDirLight);
	shader.setBool("hasPointLight", hasPointLight);
	shader.setBool("hasSpotLight", hasSpotLight);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, 0.01 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, 0.01 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, 0.01 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, 0.01 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, 0.01 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, 0.01 * deltaTime);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
	//if you press the Esc key, the window will close
	if (key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window, GL_TRUE);

	//IJKL buttons to move the model
	if (key == GLFW_KEY_I) //I moves the object along the +Y axis
		modl_move.z += 1;

	if (key == GLFW_KEY_K) //K moves the object along the -Y
		modl_move.z -= 1;

	if (key == GLFW_KEY_J) //J moves the object along the +X axis
		modl_move.y += 1;

	if (key == GLFW_KEY_L) //L moves the object along the -X axis
		modl_move.y -= 1;

	if (key == GLFW_KEY_PAGE_UP) //PgUp moves the object along the +Z axis
		modl_move.x += 1;

	if (key == GLFW_KEY_PAGE_DOWN) //PgDown moves the object along the -Z axis
		modl_move.x -= 1;

	//BNE buttons to rotate the model
	if (key == GLFW_KEY_B) //B rotates the object about the X axis
		model = glm::rotate(model, glm::radians(5.f), glm::vec3(0, 1, 0));

	if (key == GLFW_KEY_N) //N rotates the object about the Y axis,
		model = glm::rotate(model, glm::radians(5.f), glm::vec3(1, 0, 0));

	if (key == GLFW_KEY_E) //rotates the object about the Z axis
		model = glm::rotate(model, glm::radians(5.f), glm::vec3(0, 0, 1));

	//OP buttons to scale up and down
	if (key == GLFW_KEY_O) //O scales up the object by a factor of 10%
		model = glm::scale(model, glm::vec3(1.1f));

	if (key == GLFW_KEY_P) //P scales up the object by a factor of -10%
		model = glm::scale(model, glm::vec3(0.9f));

	//toggle the directional light on/off
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) 
	{
		hasDirLight = !hasDirLight;
	}

	//toggle the point light(s) on/off
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		hasPointLight = !hasPointLight;
	}

	//toggle the spot light on/off
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		hasSpotLight = !hasSpotLight;
	}

}


//LEFT mouse button + drag up and down, moves the camera further and closer to the object
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);

	//bool lbutton_pressed;

	//if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	//	lbutton_pressed = true;
	//else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	//	lbutton_pressed = false;

	////moving forward / backward while left button is pressed
	//if (lbutton_pressed) {
	//	if (lastY - ypos > 0) { //mouse going up, camera moves backward
	//		cam_pos -= cam_dir;
	//		lastY = ypos;
	//	}

	//	if (lastY - ypos < 0) { //mouse going down, camera moves forward
	//		cam_pos += cam_dir;
	//		lastY = ypos;
	//	}
	//}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
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
