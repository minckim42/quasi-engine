#include "buffer.h"

VertexArray::VertexArray()
{
	glCreateVertexArrays(1, &id);
}

VertexArray::~VertexArray()
{
	if (id)
		glDeleteVertexArrays(1, &id);
}

VertexArray::VertexArray(VertexArray&& other) noexcept:
	id(other.id), element_count(other.element_count)
{}

VertexArray& VertexArray::operator=(VertexArray& other) noexcept
{
	id = other.id;
	element_count = other.element_count;
	other.id = 0;
	return *this;
}

void VertexArray::set_element_count(size_t size)
{
	element_count = size;
}

void VertexArray::draw() const
{
	glBindVertexArray(id);
	glDrawElements(GL_TRIANGLES, element_count, GL_UNSIGNED_INT, nullptr);
}

VertexArray::operator GLuint()
{
	return id;
}

VertexArray::operator GLuint() const
{
	return id;
}
