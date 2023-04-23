#pragma once
#include <vector>
#include <glad/glad.h>
#include "shader.h"
#include "vertex_attrib.h"


#include <iostream>
#include <type_traits>
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

	void send_to_device(const std::vector<DataType>& data)
	{
		element_count = data.size();
		if constexpr (std::is_same_v<DataType, BoneIndices>)
		{
			std::cout << sizeof(DataType) << std::endl;
		}
		glNamedBufferData(id, element_count * sizeof(DataType), data.data(), GL_STATIC_DRAW);
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