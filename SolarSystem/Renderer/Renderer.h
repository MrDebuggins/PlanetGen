#pragma once

#include <string>

#include "Camera.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>


/**
 * \brief PI constant
 */
#ifndef PI
	#define PI glm::pi<float>()
#endif


// default screen size
constexpr float d_width = 700;
constexpr float d_height = 400;


/**
 * \brief Rendering manager class
 */
class Renderer
{
	// window parameters
	static float windowW;
	static float windowH;

	// default shader program
	static GLuint defaultShaderProgram;

	// rendering matrices
	static glm::mat4 projectionMatrix;

	// light source position
	static glm::vec3 lightPos;

	// camera used for rendering
	static Camera* camera;

public:
	~Renderer()
	{
		camera = nullptr;
	}

	/**
	 * \brief Initializes all common parameters and settings
	 */
	static void init(Camera* camera);

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

	/**
	 * \brief Get default shader program, made from vertex and fragment shaders only.
	 * \return shader program
	 */
	static GLuint getDefaultShaderProgram();

	/**
	 * \brief Set scene light source position (only one for now).
	 * \param light_pos light source position
	 */
	static void setLightPos(glm::vec3 light_pos);

	/**
	 * \brief Callback for changing window size
	 * \param w width
	 * \param h height
	 */
	static void reshape(int w, int h);

	/**
	 * \brief Set shader program to be used for draw calls
	 * \param shaderProgram shader program identifier
	 */
	static void useShaderProgram(GLuint shaderProgram);

	/**
	 * \brief Get used camera's view matrix
	 * \return view matrix
	 */
	static glm::mat4x4 getViewMatrix();

	/**
	 * \brief Get current projection matrix
	 * \return projection matrix
	 */
	static glm::mat4x4 getProjectionMatrix();
};