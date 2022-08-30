#include "Shader.h"

using namespace feed;

// --------------------------------------------------------------------------------------
void Shader::init()
{
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
		return;
	}


	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fs, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		return;
	}


	id = glCreateProgram();
	glAttachShader(id, fs);
	glAttachShader(id, vs);
	glLinkProgram(id);
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::LINKER::COMPILATION_FAILED\n" << infoLog << std::endl;
		return;
	}

	glDeleteShader(vs);
	glDeleteShader(fs);
}

// --------------------------------------------------------------------------------------
void Shader::bind(double t)
{
	glUseProgram(id);
	GLint tLoc = glGetUniformLocation(id, "t");
	glUniform1f(tLoc, t);
}

// --------------------------------------------------------------------------------------
void Shader::unbind()
{
	glUseProgram(0);
}
