#include "App.h"

App::App()
{
	camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	lightPos = glm::vec4(1.2f, 1.0f, 2.0f, 1.0f);
	lightDir = glm::vec4(-0.2f, -1.0f, -0.3f, 0.0f);
	pointLightPositions = {
						glm::vec3(0.7f,  0.2f,  2.0f),
						glm::vec3(2.3f, -3.3f, -4.0f),
						glm::vec3(-4.0f,  2.0f, -12.0f),
						glm::vec3(0.0f,  0.0f, -3.0f) };
}

void App::initialize()
{
	glfwSetFramebufferSizeCallback(window, CallbackHandler::framebuffer_size_callback_dispatch);
	glfwSetKeyCallback(window, CallbackHandler::key_callback_dispatch);
	glfwSetCursorPosCallback(window, CallbackHandler::cursor_pos_callback_dispatch);
	glfwSetScrollCallback(window, CallbackHandler::scroll_callback_dispatch);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetMouseButtonCallback(window, mouse_button_callback);
	//glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
}

App::~App()
{
	delete camera;
	camera = nullptr;
}

void App::renderScene(const Shader& shader)
{
	shader.setVector("spotLight.position", glm::vec4(camera->GetPosition(), 1.0f));
	shader.setVector("spotLight.direction", camera->GetFront());

	shader.setVector("view_position", camera->GetPosition());

	glm::mat4 proj_matrix = glm::mat4(1.0f);
	proj_matrix = glm::perspective(glm::radians(camera->GetZoom()), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);
	shader.setMatrix4("proj_matrix", proj_matrix);

	glm::mat4 view_matrix = camera->GetViewMatrix();
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


void App::processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->ProcessKeyboard(FORWARD, 0.01 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->ProcessKeyboard(BACKWARD, 0.01 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->ProcessKeyboard(LEFT, 0.01 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->ProcessKeyboard(RIGHT, 0.01 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		camera->ProcessKeyboard(UP, 0.01 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		camera->ProcessKeyboard(DOWN, 0.01 * deltaTime);
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

void App::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
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

	camera->ProcessMouseMovement(xoffset, yoffset);

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

void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera->ProcessMouseScroll(yoffset);
}