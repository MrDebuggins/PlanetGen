#pragma once
#include <string>

#include <GL/glew.h>


/**
 * \brief Class for managing a shader
 */
class Shader
{
public:
	/**
	 * \brief Compiles the shader
	 * \return shader identifier
	 */
	static GLuint compileShader(const GLuint type, const std::string& file);

	/**
	 * \brief Create a shader program using provided shader files
	 * \param vs_file vertex shader file path
	 * \param fs_file fragment shader file path
	 * \param tcs_file tessellation control shader file path
	 * \param tes_file tessellation evaluation shader file path
	 * \param gs_file geometry shader file path
	 * \return shader program identifier
	 */
	static GLuint createShaderProgram(const std::string& vs_file, const std::string& fs_file, const std::string& tcs_file = "", const std::string& tes_file = "", const std::string& gs_file = "");
};
