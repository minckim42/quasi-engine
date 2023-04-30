#include "mesh.hpp"
#include "utilities/glm_to_string.hpp"
#include "vertex_attrib.hpp"

void Mesh::enable_attrib()
{
	if (!position.empty())
	{
		position_buffer.send_to_device(position);
		::enable_attrib<VERTEX_ATTRIB::POSITION>(vertex_array, position_buffer);
	}
	if (!normal.empty())
	{
		normal_buffer.send_to_device(normal);
		::enable_attrib<VERTEX_ATTRIB::NORMAL>(vertex_array, normal_buffer);
	}
	if (!tangent.empty())
	{
		tangent_buffer.send_to_device(tangent);
		::enable_attrib<VERTEX_ATTRIB::TANGENT>(vertex_array, tangent_buffer);
	}
	if (!bi_tangent.empty())
	{
		bi_tangent_buffer.send_to_device(bi_tangent);
		::enable_attrib<VERTEX_ATTRIB::BI_TANGENT>(vertex_array, bi_tangent_buffer);
	}
	if (!tex_coord.empty())
	{
		tex_coord_buffer.send_to_device(tex_coord);
		::enable_attrib<VERTEX_ATTRIB::TEX_COORD>(vertex_array, tex_coord_buffer);
	}
	if (!weights.empty())
	{
		weights_buffer.send_to_device(weights);
		::enable_attrib<VERTEX_ATTRIB::WEIGHTS>(vertex_array, weights_buffer);
	}
	if (!bone_indices.empty())
	{
		bone_indices_buffer.send_to_device(bone_indices);
		::enable_attrib<VERTEX_ATTRIB::BONE_INDICES>(vertex_array, bone_indices_buffer);
	}
	element_buffer.send_to_device(indices);
	glVertexArrayElementBuffer(vertex_array, element_buffer);
	vertex_array.set_element_count(element_buffer.get_element_count());
}

void Mesh::draw() const
{
	vertex_array.draw();
}

std::pair<glm::vec3, glm::vec3> Mesh::getBoundingBox() const
{
	using namespace std::chrono_literals;

	std::pair<glm::vec3, glm::vec3> result = {
		{FLT_MAX, FLT_MAX, FLT_MAX},
		{-FLT_MAX, -FLT_MAX, -FLT_MAX}
	};
	if (position.empty())
	{
		return result;
	}
	for (const auto& pos: position)
	{
		result.first = glm::min(result.first, pos);
		result.second = glm::max(result.second, pos);
	}
	return result;
}
