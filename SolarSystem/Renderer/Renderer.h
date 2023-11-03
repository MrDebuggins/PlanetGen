#pragma once

#include "IRandable.h"
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
	float windowW = d_width;
	float windowH = d_height;


	float tmp = 0.0f;

	// default rendering shader
	Shader defaultShader = Shader();

	// rendering matrices
	glm::mat4 projectionMatrix = glm::perspective(PI / 4, windowW / windowH, 0.1f, 10000000000.0f);
	glm::mat4 viewMatrix;
	glm::mat4 modelMatrix;

	// light source position
	glm::vec3 lightPos;

	GLuint vaoObj;								// vertex array object (data descriptor)
	GLuint vboObj;								// vertex buffer object (vertex data)
	GLuint eboObj;								// element buffer object (primitives data)

public:
	Camera* camera;

	/**
	 * \brief Initializes all common parameters and settings
	 */
	Renderer(Camera* camera);

	void attachShader(GLuint programme, GLuint vs, GLuint fs);

	void registerObject(IRandable* obj);

	void setLightPos(glm::vec3 light_pos);

	void draw(IRandable* obj);

	void reshape(int w, int h);
};
