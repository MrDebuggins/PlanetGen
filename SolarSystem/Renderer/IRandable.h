#pragma once
#include "Shader.h"

#include <vector>

#include <glm/detail/type_vec.hpp>
#include <glm/detail/type_vec3.hpp>


class IRandable
{
protected:
	GLuint vbo;
	GLuint vao;
	GLuint ebo;

	glm::vec3 position;
	glm::vec3 cameraPos;
public:
	//~IRandable() = default;

	virtual void update() = 0;
	
	virtual Shader* getObjShader() = 0;

	virtual std::vector<glm::vec3>* getVertexArray() = 0;

	virtual int vertexArraySize()
	{
		return 0;
	}

	virtual std::vector<unsigned int>* getPrimitivesArray() = 0;

	void setVbo(GLuint vbo)
	{
		this->vbo = vbo;
	}

	void setVao(GLuint vao)
	{
		this->vao = vao;
	}

	void setEbo(GLuint ebo)
	{
		this->ebo = ebo;
	}

	GLuint getVbo()
	{
		return vbo;
	}

	GLuint getVao()
	{
		return vao;
	}

	GLuint getEbo()
	{
		return ebo;
	}

	virtual glm::vec3 getPosition()
	{
		return position;
	}

	void setPosition(glm::vec3 pos)
	{
		position = pos;
	}

	virtual void setCameraPos(const glm::vec3 pos)
	{
		cameraPos = pos;
	}
};
