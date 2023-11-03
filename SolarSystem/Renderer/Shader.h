#pragma once
#include <string>

#include <GL/glew.h>


/**
 * \brief Class for managing a single set of vertex and fragment shaders
 */
class Shader
{
	std::string vsFile;					//Path to vertex shader file
	std::string fsFile;					//Path to fragment shader file

	GLuint shaderProgramme = 0;			//opengl shader programme
	GLuint vert = 0;						//opengl vertex shader
	GLuint frag = 0;						//opengl fragment shader

public:
	/**
	 * Constructor with default shaders
	 */
	Shader();

	/**
	 * \brief Constructor with custom shaders
	 * \param vs_file Path to vertex shader file
	 * \param fs_file Path to fragment shader file
	 */
	Shader(const std::string& vs_file, const std::string& fs_file);

	~Shader()
	{
		glDeleteProgram(shaderProgramme);
	}

	/**
	 * \brief Creates shaders and shader programme used for rendering
	 * \return 0 - successful, non 0 - error
	 */
	int createShaderProgramme();

	GLuint getShaderProgramme() const { return shaderProgramme; }

	GLuint getVertexShader() const { return vert; }

	GLuint getFragmentShader() const { return frag; }
};
