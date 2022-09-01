#include "Window.h"

using namespace feed;

// --------------------------------------------------------------------------------------
Window::Window() : framerate(90), clearColor(0.2f, 0.3f, 0.3f), width(1024), height(768), xpos(100), ypos(100)
{
	if (glfwInit() == GLFW_FALSE) {
		glfwTerminate();
		throw std::runtime_error("Couldn't initialize GLFW.");
	}

	glfwSetErrorCallback([](int error, const char* description) {
		std::cout << "[GLFW error] " << description << std::endl;
	});


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	
	std::string title = "FeedPlayer";
	if (true) {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		window = glfwCreateWindow(mode->width, mode->height, title.c_str(), monitor, NULL);
		glViewport(0, 0, mode->width, mode->height);
	} else {
		window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
		glViewport(0, 0, width, height);
		glfwSetWindowPos(window, xpos, ypos);
	}

	if (!window) {
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window");
	}

	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, this);
	glfwSwapInterval(1);

	
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
Window::~Window() 
{
	videos[0].stop();
	videos[1].stop();

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


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
void Window::set_fullscreen(bool fullscreen)
{
	if (is_fullscreen() == fullscreen)
		return;

	if (fullscreen)
	{
		glfwGetWindowPos(window, &xpos, &ypos);
		glfwGetWindowSize(window, &width, &height);

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		std::cout << "setting fullscreen " << mode->width << ", " << mode->height << ", " << mode->refreshRate << std::endl;
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	else
	{
		glfwSetWindowMonitor(window, nullptr, xpos, ypos, width, height, 0);
	}
}


// --------------------------------------------------------------------------------------
void Window::setup()
{
	//set_size(1920, 1080);
	textures.resize(2);
	meshes.resize(2);

	meshes[0].make_quad(0, 0, 1, 1);
	meshes[1].make_quad(-1, 0, 1, 1);

	shader.init();



	videos.resize(2);

	videos[0].load("data\\video01.mp4");
	videos[0].play();

	videos[1].load("data\\video02.mkv");
	videos[1].play();


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); // (void)io;

	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 120";
	ImGui_ImplOpenGL3_Init(glsl_version);
}


// --------------------------------------------------------------------------------------
void Window::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	std::stringstream title;
	title << (1 / deltaTime) << "fps";
	set_title(title.str());

	double time = glfwGetTime();
	videos[0].update(textures[0]);
	videos[1].update(textures[1]);


	shader.bind(time);

	textures[0].bind();
	meshes[0].draw();
	textures[0].unbind();

	textures[1].bind();
	meshes[1].draw();
	textures[1].unbind();

	shader.unbind();


	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();


	static float f = 0.0f;
	static int counter = 0;

	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
	{
		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::End();


	ImGui::Render();

	//GLint last_program;
	//glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	//glUseProgram(0);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	//glUseProgram(last_program);

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
			videos[0].toggle_pause();
			videos[1].toggle_pause();
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
void Window::on_mouse(double xpos, double ypos) 
{
	//std::cout << "(" << xpos << ", " << ypos << ")" << std::endl;
}
