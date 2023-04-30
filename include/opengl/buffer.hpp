#pragma once
#include <type_traits>
#include <vector>
#include <glad/glad.h>
#include "shader.hpp"

enum class DRAW_USAGE
{
	STREAM_DRAW = GL_STREAM_DRAW,
	STREAM_READ = GL_STREAM_READ,
	STREAM_COPY = GL_STREAM_COPY,
	STATIC_DRAW = GL_STATIC_DRAW,
	STATIC_READ = GL_STATIC_READ,
	STATIC_COPY = GL_STATIC_COPY,
	DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
	DYNAMIC_READ = GL_DYNAMIC_READ,
	DYNAMIC_COPY = GL_DYNAMIC_COPY,
};

template <typename DataType>
class Buffer
{
public:

	Buffer()
	{
		glCreateBuffers(1, &id);
	}

	Buffer(const std::vector<DataType>& vertices)
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
		if (id)
			glDeleteBuffers(1, &id);
		id = other.id;
		element_count = other.element_count;
		other.id = 0;
	}

	void send_to_device(const std::vector<DataType>& data, DRAW_USAGE draw_usage = DRAW_USAGE::STATIC_DRAW)
	{
		element_count = data.size();
		glNamedBufferData(id, element_count * sizeof(DataType), data.data(), static_cast<GLenum>(draw_usage));
	}

	void bind_as_unifrom(GLuint binding) const
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, id);
	}

	GLuint get() const 
	{
		return id;
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

class VertexArray
{
public:
	VertexArray();
	~VertexArray();
	VertexArray(const VertexArray&) = delete;
	VertexArray& operator=(const VertexArray&) = delete;
	VertexArray(VertexArray&& other) noexcept;
	VertexArray& operator=(VertexArray& other) noexcept;

	void set_element_count(size_t size);
	void draw() const;
	GLuint get() const;
	operator GLuint();
	operator GLuint() const;

private:
	GLuint id = 0;
	unsigned int element_count = 0;
};