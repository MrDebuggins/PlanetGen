#include "Shader.h"

#include <fstream>
#include <iostream>


void printShaderInfoLog(const GLuint obj)
{
	int infoLogLength = 0;
	int charsWritten = 0;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infoLogLength);

	if (infoLogLength > 0)
	{
		const auto infoLog = static_cast<char*>(malloc(infoLogLength));
		glGetShaderInfoLog(obj, infoLogLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

void printProgramInfoLog(const GLuint obj)
{
	int infoLogLength = 0;
	int charsWritten = 0;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infoLogLength);

	if (infoLogLength > 0)
	{
		const auto infoLog = static_cast<char*>(malloc(infoLogLength));
		glGetProgramInfoLog(obj, infoLogLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

std::string textFileRead(const char* fn)
{
	std::ifstream file(fn);
	std::string fileText;
	while (file.good()) {
		std::string line;
		std::getline(file, line);
		fileText.append(line + "\n");
	}
	return fileText;
}

Shader::Shader()
{
	vsFile = "default_vertex.vert";
	fsFile = "default_fragment.frag";
}

Shader::Shader(const std::string& vs_file, const std::string& fs_file)
{
	this->vsFile = vs_file;
	this->fsFile = fs_file;

	createShaderProgramme();
}

int Shader::createShaderProgramme()
{
	// read shaders code from files
	const std::string vs_text = textFileRead(vsFile.c_str());
	const std::string fs_text = textFileRead(fsFile.c_str());
	const char* vertex_shader = vs_text.c_str();
	const char* fragment_shader = fs_text.c_str();

	// create and compile vertex shader
	vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &vertex_shader, nullptr);
	glCompileShader(vert);

	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vert, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// create and compile fragment shader
	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &fragment_shader, nullptr);
	glCompileShader(frag);

	// check for shader compile errors
	glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(frag, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	shaderProgramme = glCreateProgram();

	// print compile logs
	printShaderInfoLog(frag);
	printShaderInfoLog(vert);
	printProgramInfoLog(shaderProgramme);

	return success;
}

