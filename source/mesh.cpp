#include "../include/mesh.h"
// #include "mesh.h"
#include <iostream>
#include "glm_to_string.h"

void Mesh::enable_attrib()
{
	if (!position.empty())
	{
		position_buffer.send_to_device(position);
		enable_attrib(
			position_buffer,
			static_cast<GLuint>(VERTEX_ATTRIB_LOCATION::POSITION),
			static_cast<GLuint>(VERTEX_ATTRIB_LOCATION::POSITION),
			sizeof(Position) / sizeof(float),
			GL_FLOAT, GL_FALSE, 0
		);
	}
	if (!normal.empty())
	{
		normal_buffer.send_to_device(normal);
		enable_attrib(
			normal_buffer,
			static_cast<GLuint>(VERTEX_ATTRIB_LOCATION::NORMAL),
			static_cast<GLuint>(VERTEX_ATTRIB_LOCATION::NORMAL),
			sizeof(Normal) / sizeof(float),
			GL_FLOAT, GL_FALSE, 0
		);
	}
	if (!tangent.empty())
	{
		tangent_buffer.send_to_device(tangent);
		enable_attrib(
			tangent_buffer,
			static_cast<GLuint>(VERTEX_ATTRIB_LOCATION::TANGENT),
			static_cast<GLuint>(VERTEX_ATTRIB_LOCATION::TANGENT),
			sizeof(Tangent) / sizeof(float),
			GL_FLOAT, GL_FALSE, 0
		);
	}
	if (!bi_tangent.empty())
	{
		bi_tangent_buffer.send_to_device(bi_tangent);
		enable_attrib(
			bi_tangent_buffer,
			static_cast<GLuint>(VERTEX_ATTRIB_LOCATION::BI_TANGENT),
			static_cast<GLuint>(VERTEX_ATTRIB_LOCATION::BI_TANGENT),
			sizeof(BiTangent) / sizeof(float),
			GL_FLOAT, GL_FALSE, 0
		);
	}
	if (!tex_coord.empty())
	{
		tex_coord_buffer.send_to_device(tex_coord);
		enable_attrib(
			tex_coord_buffer,
			static_cast<GLuint>(VERTEX_ATTRIB_LOCATION::TEX_COORD),
			static_cast<GLuint>(VERTEX_ATTRIB_LOCATION::TEX_COORD),
			sizeof(TexCoord) / sizeof(float),
			GL_FLOAT, GL_FALSE, 0
		);
	}
	if (!weights.empty())
	{
		weights_buffer.send_to_device(weights);
		enable_attrib(
			weights_buffer,
			static_cast<GLuint>(VERTEX_ATTRIB_LOCATION::WEIGHTS),
			static_cast<GLuint>(VERTEX_ATTRIB_LOCATION::WEIGHTS),
			WEIGHT_COUNT,
			GL_FLOAT, GL_FALSE, 0
		);
	}
	if (!bone_indices.empty())
	{
		bone_indices_buffer.send_to_device(bone_indices);
		enable_attrib(
			bone_indices_buffer,
			static_cast<GLuint>(VERTEX_ATTRIB_LOCATION::BONE_INDICES),
			static_cast<GLuint>(VERTEX_ATTRIB_LOCATION::BONE_INDICES),
			WEIGHT_COUNT,
			GL_UNSIGNED_INT, GL_FALSE, 0
		);
	}
	element_buffer.send_to_device(indices);
	glVertexArrayElementBuffer(vertex_array, element_buffer);
	vertex_array.set_element_count(element_buffer.get_element_count());
}

void Mesh::draw(
	const Shader& shader, 
	const glm::mat4& view,
	const glm::mat4& projection,
	const glm::mat4& model
) const
{
	using namespace std::string_view_literals;
	shader.use();
	shader.set("projection"sv, projection);
	shader.set("view"sv, view);
	shader.set("model"sv, model);
	if (material) material->bind(shader);
	vertex_array.draw();
}

void Mesh::draw(const Shader& shader, 
	unsigned int idx_animation,
	Microseconds time,
	const glm::mat4& view, 
	const glm::mat4& projection,
	const glm::mat4& model
)
{
	using namespace std::string_view_literals;
	shader.use();
	shader.set("projection"sv, projection);
	shader.set("view"sv, view);
	shader.set("model"sv, model);

	bone.update(transform, idx_animation, time);

	shader.set("transform"sv, *transform.data(), transform.size());
	if (material) material->bind(shader);
	vertex_array.draw();
}

std::pair<glm::vec3, glm::vec3> Mesh::getBoundingBox() const
{
	std::pair<glm::vec3, glm::vec3> result = {
		{FLT_MAX, FLT_MAX, FLT_MAX},
		{-FLT_MAX, -FLT_MAX, -FLT_MAX}
	};
	if (position.empty())
		return result;
	for (const auto& pos: position)
	{
		result.first = glm::min(result.first, pos);
		result.second = glm::max(result.second, pos);
	}
	return result;
}
