//modified from http://learnopengl.com/

#include "App.h"
using namespace std;

int init(App * app);

// The MAIN function, from here we start the application and run the game loop.
int main()
{
	App* application = new App();
	application->initialize();

	if (init(application) != 0)
		return EXIT_FAILURE;

	stbi_set_flip_vertically_on_load(true);

	// 2. use our shader program when we want to render an object
	// Build and compile our shader program
	// Vertex shader.
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
	
	shader.setVector("dirLight.position", application->lightPos);
	shader.setVector("dirLight.direction", application->lightDir);
	shader.setVector("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	shader.setVector("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
	shader.setVector("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

	for (GLuint i = 0; i < (sizeof(application->pointLightPositions)/sizeof(application->pointLightPositions[0])) - 1; i++)
	{
		string number = to_string(i);

		shader.setVector("pointLights[" + number + "].position", application->pointLightPositions[i]);
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

	while (!glfwWindowShouldClose(application->window))
	{
		float currentFrame = glfwGetTime();
		application->deltaTime = currentFrame - application->lastFrame;
		application->lastFrame = currentFrame;

		application->processInput(application->window);

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
			application->renderScene(depth_shader);
			object.Draw(depth_shader);
			plane.Draw(depth_shader);
			// shader stuff ^^^^^^^^^^^^^^^

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		//Second pass : render scene as normal with shadow mapping (using depth map)
		// --------------------------------------------------------------
		glViewport(0, 0, application->WIDTH, application->HEIGHT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// shader stuff vvvvvvvvvvv
		shader.use();

		shader.setMatrix4("lightSpace_matrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		application->renderScene(shader);
		object.Draw(shader);
		plane.Draw(shader);
		// shader stuff ^^^^^^^^^^^^^^^

		// Swap the screen buffers
		glfwSwapBuffers(application->window);
		glfwPollEvents();
	}

	/*glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);*/

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();

	delete application;
	return 0;
}

int init(App * application) {
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
	application->window = glfwCreateWindow(application->WIDTH, application->HEIGHT, "OpenGL_Project", nullptr, nullptr);

	if (nullptr == application->window)
	{
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(application->window);


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
