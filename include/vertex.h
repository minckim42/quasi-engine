#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 tex_coord;

	static void enable_attrib(unsigned int vao)
	{
		glEnableVertexArrayAttrib(vao, 0);
		glVertexArrayAttribBinding(vao, 0, 0);
		glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
		
		glEnableVertexArrayAttrib(vao, 1);
		glVertexArrayAttribBinding(vao, 1, 0);
		glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
		
		glEnableVertexArrayAttrib(vao, 2);
		glVertexArrayAttribBinding(vao, 2, 0);
		glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, tex_coord));
	}
};
