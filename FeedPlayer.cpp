#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

#include "Window.h"
#include "Video.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"

int main() {

	feed::Video video;
	feed::Mesh mesh;
	feed::Texture texture;
	feed::Shader shader;

	std::cout << "Current path is " << fs::current_path() << std::endl;

	if (glfwInit() == GLFW_FALSE) {
		std::cout << "Couldn't initialize GLFW." << std::endl;
		return -1;
	}

	glfwSetErrorCallback([](int error, const char* description) {
		std::cout << "[GLFW error] " << description << std::endl;
	});

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);


	GLFWwindow* window = glfwCreateWindow(800, 600, "My Title", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, &video);


	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		std::cout << "glfwSetFramebufferSizeCallback" << width << ", " << height << std::endl;
		glViewport(0, 0, width, height);
	});

	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {

		feed::Video* vid = static_cast<feed::Video*>(glfwGetWindowUserPointer(window));

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);

		switch (action) {
		case GLFW_PRESS:
			break;
		case GLFW_RELEASE:
			if (key == GLFW_KEY_SPACE) {
				std::cout << "toggling pause" << std::endl;
				vid->toggle_pause();
			}
			break;
		case GLFW_REPEAT:
			break;
		}
	});
	

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		glfwTerminate();
		std::cout << "[GLEW error] " << glewGetErrorString(err) << std::endl;
		return -1;
	}


	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);



	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"



	video.load("video.mp4");
	mesh.init();
	shader.init();
	video.play();



	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		double time = glfwGetTime();
		
		video.update(texture);
	
		shader.bind(time);
		texture.bind();
		
		mesh.draw();

		//texture.unbind();
		//shader.unbind();


		glfwSwapBuffers(window);
		glfwPollEvents();
	}



	video.stop();


	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

