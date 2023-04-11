#pragma once
#include <vector>
#include <glad/glad.h>
#include "shader.h"

template <typename V>
class Buffer
{
public:
	Buffer()
	{
		glCreateBuffers(1, &id);
	}

	Buffer(const std::vector<V>& vertices)
	{
		glCreateBuffers(1, &id);
		send_to_device(vertices);
	}

	~Buffer()
	{
		if (id)
			glDeleteBuffers(1, &id);
	}

	Buffer(const Buffer&) = delete;
	Buffer& operator=(const Buffer&) = delete;

	Buffer(Buffer&& other): id(other.id), element_count(other.element_count)
	{
		other.id = 0;
	}

	Buffer& operator=(Buffer&& other)
	{
		id = other.id;
		element_count = other.element_count;
		other.id = 0;
	}

	void send_to_device(const std::vector<V>& data)
	{
		element_count = data.size();
		glNamedBufferData(id, element_count * sizeof(V), data.data(), GL_STATIC_DRAW);
	}

	operator GLuint()
	{
		return id;
	}

	operator GLuint() const
	{
		return id;
	}

	unsigned int get_element_count() const
	{
		return element_count;
	}
private:
	GLuint id = 0;
	unsigned int element_count = 0;
};

using ElementBuffer = Buffer<unsigned int>;

template <typename V>
class VertexArray
{
public:
	VertexArray()
	{
		glCreateVertexArrays(1, &id);
		enable_attrib();
	}

	~VertexArray()
	{
		if (id)
			glDeleteVertexArrays(1, &id);
	}

	void bind(const Buffer<V>& vertex_buffer, const ElementBuffer& element_buffer)
	{
		glVertexArrayVertexBuffer(id, 0, vertex_buffer, 0, sizeof(V));
		glVertexArrayElementBuffer(id, element_buffer);
		element_count = element_buffer.get_element_count();
	}

	void draw(const Shader& shader) const
	{
		glBindVertexArray(id);
		glDrawElements(GL_TRIANGLES, element_count, GL_UNSIGNED_INT, nullptr);
	}

	operator GLuint()
	{
		return id;
	}

	operator GLuint() const
	{
		return id;
	}

private:

	void enable_attrib()
	{
		V::enable_attrib(id);
	}

	GLuint id = 0;
	unsigned int element_count = 0;
};

