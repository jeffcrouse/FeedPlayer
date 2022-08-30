#include <filesystem>
#include <sstream>
#include "Window.h"

using namespace feed;


// --------------------------------------------------------------------------------------
Window::Window() : framerate(90), clearColor(0.2f, 0.3f, 0.3f), width(800), height(600), xpos(100), ypos(100)
{
	if (glfwInit() == GLFW_FALSE) {
		glfwTerminate();
		throw std::runtime_error("Couldn't initialize GLFW.");
	}

	glfwSetErrorCallback([](int error, const char* description) {
		std::cout << "[GLFW error] " << description << std::endl;
		});

	std::cout << "Current path is " << std::filesystem::current_path() << std::endl;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	monitor = glfwGetPrimaryMonitor();

	window = glfwCreateWindow(width, height, "My Title", NULL, NULL);
	if (!window) {
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window");
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, this);

	glfwSetWindowPos(window, xpos, ypos);

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		std::cout << "glfwSetFramebufferSizeCallback " << width << ", " << height << std::endl;
		glViewport(0, 0, width, height);
	});

	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		feed::Window* w = static_cast<feed::Window*>(glfwGetWindowUserPointer(window));
		w->on_key(key, scancode, action, mods);
	});

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		feed::Window* w = static_cast<feed::Window*>(glfwGetWindowUserPointer(window));
		w->on_mouse(xpos, ypos);
	});

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		glfwTerminate();
		char str[150];
		sprintf_s(str, "[GLEW error] %s", glewGetErrorString(err));
		throw std::runtime_error(str);
	}
	//glfwSwapInterval(0);
	std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
	setup();
}


// --------------------------------------------------------------------------------------
void Window::setup()
{
	set_size(1920, 1080);

	texture.init();
	video.load("video.mp4");
	mesh.init();
	shader.init();
	video.play();
}


// --------------------------------------------------------------------------------------
void Window::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	std::stringstream title;
	title << (1 / deltaTime) << "fps";
	set_title(title.str());

	double time = glfwGetTime();
	video.update(texture);

	shader.bind(time);
	texture.bind();

	mesh.draw();

	texture.unbind();
	shader.unbind();
}

// --------------------------------------------------------------------------------------
Window::~Window() 
{
	video.stop();
	glfwDestroyWindow(window);
	glfwTerminate();
}


// --------------------------------------------------------------------------------------
void Window::run() 
{
	while (!glfwWindowShouldClose(window))
	{
		double currentTime = glfwGetTime(); // The current value, in seconds, or zero if an error occurred.
		deltaTime = currentTime - lastTime;
	
		//if(deltaTime > 1/framerate) {
			draw();
			lastTime = currentTime;
		//}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}


// --------------------------------------------------------------------------------------
void Window::set_title(std::string title)
{
	glfwSetWindowTitle(window, title.c_str());
}

// --------------------------------------------------------------------------------------
void Window::set_size(unsigned int width, unsigned int height) 
{
	glfwSetWindowSize(window, width, height);
}

// --------------------------------------------------------------------------------------
void Window::on_key(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	switch (action) {
	case GLFW_PRESS:
		break;
	case GLFW_RELEASE:
		if (key == GLFW_KEY_SPACE) {
			std::cout << "toggling pause" << std::endl;
			video.toggle_pause();
		}
		if (key == GLFW_KEY_F) {
			std::cout << "toggling fullscreen" << std::endl;
			toggle_fullscreen();
		}
		break;
	case GLFW_REPEAT:
		break;
	}
}


// --------------------------------------------------------------------------------------
void Window::on_mouse(double xpos, double ypos) {
	//std::cout << "(" << xpos << ", " << ypos << ")" << std::endl;
}


// --------------------------------------------------------------------------------------
void Window::set_fullscreen(bool fullscreen)
{
	if (is_fullscreen() == fullscreen)
		return;

	if (fullscreen)
	{
		glfwGetWindowPos(window, &xpos, &ypos);
		glfwGetWindowSize(window, &width, &height);

		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		std::cout << "setting fullscreen " << mode->width << ", " << mode->height << ", " << mode->refreshRate << std::endl;
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	else
	{
		glfwSetWindowMonitor(window, nullptr, xpos, ypos, width, height, 0);
	}
}