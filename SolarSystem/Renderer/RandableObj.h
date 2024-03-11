#pragma once

#include "Renderer.h"

#include <vector>

#include <glm/detail/type_vec.hpp>
#include <glm/detail/type_vec3.hpp>


/**
 * \brief Base class for randable objects using default shaders.
 */
class RandableObj
{
protected:
	// buffer identifiers
	GLuint vbo;
	GLuint vao;
	GLuint ebo;

	// shader program used by object
	GLuint shaderProgram;

	// object position in meters
	glm::vec3 position_m = glm::vec3(0, 0, 0);

	// object position
	glm::vec3 position = glm::vec3(0, 0, 0);

	// camera position
	glm::vec3 cameraPos;

	// vertices array
	std::vector<float> vertices = 
	{
		-0.1f, 0.0f, 6371000.0f,
		0.1f, 0.0f, 6371000.0f,
		0.0f, 1.0f, 6371000.0f
	};

	// primitives array
	std::vector<unsigned int> primitives = { 0,1,2 };

public:
	~RandableObj()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
	}

	/**
	 * \brief Prepare object for drawing. Called once after object is loaded, and mesh vertices and primitives
	 * are initialized.
	 */
	virtual void prepareObject()
	{
		shaderProgram = Renderer::getDefaultShaderProgram();

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, primitives.size() * sizeof(unsigned int), primitives.data(), GL_STATIC_DRAW);
	}

	/**
	 * \brief Function to be called in application loop for updating
	 */
	virtual void update() {};

	/**
	 * \brief Draw object. Before calling this function make sure to use corresponding shader program.
	 * For better performance draw consecutively objects that share the same shader program, if not getShaderProgram
	 * function can be used.
	 */
	virtual void draw()
	{
		glBindVertexArray(vao);

		// object transformations
		GLint modelMatrix = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

		// camera view matrix
		GLuint viewMatrix = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, glm::value_ptr(Renderer::getViewMatrix()));

		// camera view matrix
		GLuint projectionMatrix = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(projectionMatrix, 1, GL_FALSE, glm::value_ptr(Renderer::getProjectionMatrix()));

		// camera position
		GLint cameraPosLoc = glGetUniformLocation(shaderProgram, "cameraPos");
		glUniform3fv(cameraPosLoc, 1, glm::value_ptr(cameraPos));

		// object position
		GLint posLoc = glGetUniformLocation(shaderProgram, "pos");
		glUniform3fv(posLoc, 1, glm::value_ptr(position));

		glDrawElements(GL_TRIANGLES, primitives.size(), GL_UNSIGNED_INT, nullptr);
	}

	/**
	 * \brief Get object position
	 * \return xyz position
	 */
	glm::vec3 getPosition() const
	{
		return position;
	}

	/**
	 * \brief Set camera position for object. Needed for offsetting every object by camera position,
	 * so camera will be in the system origin, in order to draw planet scale objects
	 * \param pos camera position
	 */
	virtual void setCameraPos(const glm::vec3 pos)
	{
		cameraPos = pos;
	}

	/**
	 * \brief Return shader program used by this object
	 * \return shader program identifier
	 */
	GLuint getShaderProgram() const
	{
		return shaderProgram;
	}
};
