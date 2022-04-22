//modified from http://learnopengl.com/

#include "App.h"
#include "MenuGUI.h"
using namespace std;

int init(App * app);

// The MAIN function, from here we start the application and run the game loop.
int main()
{
	App* application = new App();
	application->initialize();

	if (init(application) != 0)
		return EXIT_FAILURE;

	application->runApp();

	delete application;
	return 0;
}

int init(App * application) {
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
	application->_window = glfwCreateWindow(application->WIDTH, application->HEIGHT, "OpenGL_Project", nullptr, nullptr);

	if (nullptr == application->_window)
	{
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(application->_window);


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
