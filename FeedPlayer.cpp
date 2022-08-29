#include <iostream>
#include <stdio.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vlc/vlc.h>
#include <mutex>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


std::mutex vlcMutex;
unsigned char* pixelBuffer;
bool needsUpdate = false;

static void* video_lock(void* data, void** p_pixels) {
	vlcMutex.lock();
	p_pixels[0] = (void*)pixelBuffer;
	return NULL;
}

static void video_unlock(void* data, void* id, void* const* p_pixels) {
	needsUpdate = true;
	vlcMutex.unlock();
}

static void video_display(void* opaque, void* picture) {

}


int main()
{
	// Set up VLC instance
	libvlc_instance_t* inst;
	const char* argv[] = { "--no-xlib", "--no-video-title-show" };
	//int argc = sizeof(argv) / sizeof(*argv);
	inst = libvlc_new(2, argv);
	if (inst == NULL) {
		std::cout << "[VLC error] " << std::endl;
		return false;
	}

	unsigned int videoWidth = 0;
	unsigned int videoHeight = 0;

	// Load up some media
	libvlc_media_t* m;
	m = libvlc_media_new_path(inst, "C:\\Users\\jeff\\Developer\\glfw_test\\video.mp4");
	libvlc_media_track_info_t* info;
	libvlc_media_parse(m);
	int i = libvlc_media_get_tracks_info(m, &info);
	for (--i; i >= 0; --i) {
		if (info[i].i_type == libvlc_track_video) {
			videoWidth = info[i].u.video.i_width;
			videoHeight = info[i].u.video.i_height;
		}
	}

	// Add it to a player
	libvlc_media_player_t* mp;
	mp = libvlc_media_player_new_from_media(m);
	libvlc_media_release(m);

	libvlc_video_get_size(mp, 0, &videoWidth, &videoHeight);

	std::cout << "videoWidth = " << videoWidth << " videoHeight = " << videoHeight << std::endl;

	std::mutex vlcMutex;
	libvlc_video_set_callbacks(mp, video_lock, video_unlock, video_display, NULL);
	libvlc_video_set_format(mp, "RV24", videoWidth, videoHeight, videoWidth * 3);

	
	pixelBuffer = new unsigned char[videoWidth * videoHeight * 3];


	libvlc_media_player_play(mp);




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


	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		std::cout << "glfwSetFramebufferSizeCallback" << width << ", " << height << std::endl;
		glViewport(0, 0, width, height);
	});

	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);

		switch (action) {
		case GLFW_PRESS:
			break;
		case GLFW_RELEASE:
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




	const char* vertex_shader =
		"#version 120\n"
		"attribute vec3 pos;"
		"attribute vec3 color;"
		"attribute vec2 tex;"

		"varying vec2 UV;"
		"varying vec3 vColor;"

		"void main() {"
		"   vColor = color;"
		"   UV = tex;"
		"   gl_Position = vec4(pos, 1.0);"
		"}";

	const char* fragment_shader =
		"#version 120\n"

		"uniform float t;"
		"uniform sampler2D tex;"

		"varying vec2 UV;"
		"varying vec3 vColor;"

		"void main() {"
		"	vec2 pos = gl_TexCoord[0].xy;"
		"	vec2 _uv = 0.25 * vec2(t,t) + UV;"
		"   gl_FragColor = texture2D(tex, _uv) * vec4(vColor, 1.0);"
		"}";

	int success;
	char infoLog[512];

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vs, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fs, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
 

	GLuint shader = glCreateProgram();
	glAttachShader(shader, fs);
	glAttachShader(shader, vs);
	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &success);
	if (!success) 
	{
		glGetProgramInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::LINKER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vs);
	glDeleteShader(fs);



	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// positions          // colors           // texture coords
		 1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);





	// create a texture 
	// -------------------------
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);






	//// load image, create texture and generate mipmaps
	//int width, height, nrChannels;
	//unsigned char* data = stbi_load("C:\\Users\\jeff\\Developer\\FeedPlayer\\Bricks_001.png", &width, &height, &nrChannels, 0);
	//if (data)
	//{
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//}
	//else
	//{
	//	std::cout << "Failed to load texture" << std::endl;
	//}
	//stbi_image_free(data);







	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		double t = glfwGetTime();

		if (vlcMutex.try_lock()) {
			if (needsUpdate) {
				glBindTexture(GL_TEXTURE_2D, texture);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, videoWidth, videoHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixelBuffer);
				glGenerateMipmap(GL_TEXTURE_2D);
				needsUpdate = false;
			}
			vlcMutex.unlock();
		}

		glUseProgram(shader);
		GLint tLoc = glGetUniformLocation(shader, "t");
		glUniform1f(tLoc, t);

		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);


	libvlc_media_player_stop(mp);
	libvlc_media_player_release(mp);


	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

