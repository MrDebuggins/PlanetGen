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

GLuint Shader::compileShader(const GLuint type, const std::string& file)
{
	const std::string text = textFileRead(file.c_str());
	const char* shader = text.c_str();

	GLuint identifier = glCreateShader(type);
	glShaderSource(identifier, 1, &shader, nullptr);
	glCompileShader(identifier);

	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(identifier, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(identifier, 512, nullptr, infoLog);
		std::cout << "ERROR::" << type << "::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	return identifier;
}

GLuint Shader::createShaderProgram(const std::string& vs_file, const std::string& fs_file, const std::string& tcs_file, const std::string& tes_file, const std::string& gs_file)
{
	GLuint program = glCreateProgram();

	// compile shaders and attach them to program
	GLuint vs = compileShader(GL_VERTEX_SHADER, vs_file);
	glAttachShader(program, vs);

	GLuint fs = compileShader(GL_FRAGMENT_SHADER, fs_file);
	glAttachShader(program, fs);

	GLuint tcs = 0;
	if (!tcs_file.empty())
	{
		tcs = compileShader(GL_TESS_CONTROL_SHADER, tcs_file);
		glAttachShader(program, tcs);
	}

	GLuint tes = 0;
	if (!tes_file.empty())
	{
		tes = compileShader(GL_TESS_EVALUATION_SHADER, tes_file);
		glAttachShader(program, tes);
	}

	GLuint gs = 0;
	if (!gs_file.empty())
	{
		gs = compileShader(GL_GEOMETRY_SHADER, gs_file);
		glAttachShader(program, gs);
	}

	// link program
	glLinkProgram(program);

	// delete shaders
	glDeleteShader(vs);
	glDeleteShader(fs);

	if(tcs != 0)
		glDeleteShader(tcs);

	if (tes != 0)
		glDeleteShader(tes);

	if (gs != 0)
		glDeleteShader(gs);

	return program;
}