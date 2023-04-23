#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <array>


#include <cassert>
constexpr unsigned int WEIGHT_COUNT = 4;

using Position = glm::vec3;
using Normal = glm::vec3;
using Tangent = glm::vec3;
using BiTangent = glm::vec3;
using TexCoord = glm::vec2;
using Color = glm::vec4;
using Weights = std::array<float, WEIGHT_COUNT>;
using BoneIndices = std::array<GLuint, WEIGHT_COUNT>;

using Rotation = glm::quat;
using Scaling = glm::vec3;

enum class VERTEX_ATTRIB
{
	POSITION = 0,
	NORMAL,
	TANGENT,
	BI_TANGENT,
	TEX_COORD,
	COLOR,
	WEIGHTS,
	BONE_INDICES,
};

template <VERTEX_ATTRIB VertexAttribLocation>
struct VertexAttrib
{};

template <>
struct VertexAttrib<VERTEX_ATTRIB::POSITION>
{
	using type = Position;
	static constexpr GLenum gl_type = GL_FLOAT;
	static constexpr unsigned int element_size = sizeof(float);
	static constexpr unsigned int element_count = sizeof(type) / element_size;
	static constexpr unsigned int location = static_cast<GLuint>(VERTEX_ATTRIB::POSITION);
};

template <>
struct VertexAttrib<VERTEX_ATTRIB::NORMAL>
{
	using type = Normal;
	static constexpr GLenum gl_type = GL_FLOAT;
	static constexpr unsigned int element_size = sizeof(float);
	static constexpr unsigned int element_count = sizeof(type) / element_size;
	static constexpr unsigned int location = static_cast<GLuint>(VERTEX_ATTRIB::NORMAL);
};

template <>
struct VertexAttrib<VERTEX_ATTRIB::TANGENT>
{
	using type = Tangent;
	static constexpr GLenum gl_type = GL_FLOAT;
	static constexpr unsigned int element_size = sizeof(float);
	static constexpr unsigned int element_count = sizeof(type) / element_size;
	static constexpr unsigned int location = static_cast<GLuint>(VERTEX_ATTRIB::TANGENT);
};

template <>
struct VertexAttrib<VERTEX_ATTRIB::BI_TANGENT>
{
	using type = BiTangent;
	static constexpr GLenum gl_type = GL_FLOAT;
	static constexpr unsigned int element_size = sizeof(float);
	static constexpr unsigned int element_count = sizeof(type) / element_size;
	static constexpr unsigned int location = static_cast<GLuint>(VERTEX_ATTRIB::BI_TANGENT);
};

template <>
struct VertexAttrib<VERTEX_ATTRIB::TEX_COORD>
{
	using type = TexCoord;
	static constexpr GLenum gl_type = GL_FLOAT;
	static constexpr unsigned int element_size = sizeof(float);
	static constexpr unsigned int element_count = sizeof(type) / element_size;
	static constexpr unsigned int location = static_cast<GLuint>(VERTEX_ATTRIB::TEX_COORD);
};

template <>
struct VertexAttrib<VERTEX_ATTRIB::COLOR>
{
	using type = Color;
	static constexpr GLenum gl_type = GL_FLOAT;
	static constexpr unsigned int element_size = sizeof(float);
	static constexpr unsigned int element_count = sizeof(type) / element_size;
	static constexpr unsigned int location = static_cast<GLuint>(VERTEX_ATTRIB::COLOR);
};

template <>
struct VertexAttrib<VERTEX_ATTRIB::WEIGHTS>
{
	using type = Weights;
	static constexpr GLenum gl_type = GL_FLOAT;
	static constexpr unsigned int element_size = sizeof(float);
	static constexpr unsigned int element_count = sizeof(type) / element_size;
	static constexpr unsigned int location = static_cast<GLuint>(VERTEX_ATTRIB::WEIGHTS);
};

template <>
struct VertexAttrib<VERTEX_ATTRIB::BONE_INDICES>
{
	using type = BoneIndices;
	static constexpr GLenum gl_type = GL_UNSIGNED_INT;
	static constexpr unsigned int element_size = sizeof(unsigned int);
	static constexpr unsigned int element_count = sizeof(type) / element_size;
	static constexpr unsigned int location = static_cast<GLuint>(VERTEX_ATTRIB::BONE_INDICES);
};

template <VERTEX_ATTRIB AttribLocation>
void enable_attrib(GLuint vao, GLuint vbo)
{
	using V = VertexAttrib<AttribLocation>;

	glEnableVertexArrayAttrib(vao, V::location);
	glVertexArrayAttribBinding(vao, V::location, V::location);
	if constexpr (std::is_same_v<BoneIndices, typename V::type>)
	{
		glVertexArrayAttribIFormat(vao, V::location, V::element_count, V::gl_type, 0);
	}
	else
	{
		glVertexArrayAttribFormat(vao, V::location, V::element_count, V::gl_type, GL_FALSE, 0);
	}
	glVertexArrayVertexBuffer(vao, V::location, vbo, 0, sizeof(V::type));
}
