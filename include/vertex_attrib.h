#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <array>

constexpr unsigned int WEIGHT_COUNT = 4;

using Position = glm::vec3;
using Normal = glm::vec3;
using Tangent = glm::vec3;
using BiTangent = glm::vec3;
using TexCoord = glm::vec2;
using Color = glm::vec4;
using Weights = std::array<float, WEIGHT_COUNT>;
using BoneIndices = std::array<GLuint, WEIGHT_COUNT>;
// using Weights = glm::vec4;
// using BoneIndices = glm::uvec4;

using Rotation = glm::quat;
using Scaling = glm::vec3;

enum class VERTEX_ATTRIB_LOCATION
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

// template <typename T>
// struct VertexAttribLocation
// {
// };

// template <>
// struct VertexAttribLocation<Position>
// {
// 	static const unsigned int value = 
// 		static_cast<unsigned int>(VERTEX_ATTRIB_LOCATION::POSITION);
// };

// template <>
// struct VertexAttribLocation<Normal>
// {
// 	static const unsigned int value = 
// 		static_cast<unsigned int>(VERTEX_ATTRIB_LOCATION::NORMAL);
// };

// template <>
// struct VertexAttribLocation<Tangent>
// {
// 	static const unsigned int value = 
// 		static_cast<unsigned int>(VERTEX_ATTRIB_LOCATION::TANGENT);
// };

// template <>
// struct VertexAttribLocation<BiTangent>
// {
// 	static const unsigned int value = 
// 		static_cast<unsigned int>(VERTEX_ATTRIB_LOCATION::BI_TANGENT);
// };

// template <>
// struct VertexAttribLocation<TexCoord>
// {
// 	static const unsigned int value = 
// 		static_cast<unsigned int>(VERTEX_ATTRIB_LOCATION::TEX_COORD);
// };

// template <>
// struct VertexAttribLocation<Color>
// {
// 	static const unsigned int value = 
// 		static_cast<unsigned int>(VERTEX_ATTRIB_LOCATION::COLOR);
// };

// template <>
// struct VertexAttribLocation<Weights>
// {
// 	static const unsigned int value = 
// 		static_cast<unsigned int>(VERTEX_ATTRIB_LOCATION::WEIGHTS);
// };

// template <>
// struct VertexAttribLocation<BoneIndices>
// {
// 	static const unsigned int value = 
// 		static_cast<unsigned int>(VERTEX_ATTRIB_LOCATION::BONE_INDICES);
// };