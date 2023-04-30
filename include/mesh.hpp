#pragma once
#include <memory>
#include "utilities/definition.hpp"
#include "vertex_attrib.hpp"
#include "material.hpp"
#include "opengl/buffer.hpp"
#include "bone.hpp"

class Drawable
{
public:
	virtual void draw(
		const glm::mat4& model = glm::mat4(1.0f)
	) const = 0;
	virtual void draw(
		unsigned int idx_animation,
		Microseconds time,
		const glm::mat4& model = glm::mat4(1.0f)
	) = 0;
};

class Mesh
{
public:
	std::vector<Position> position;
	std::vector<Normal> normal;
	std::vector<Tangent> tangent;
	std::vector<BiTangent> bi_tangent;
	std::vector<TexCoord> tex_coord;
	std::vector<Color> color;
	std::vector<Weights> weights;
	std::vector<BoneIndices> bone_indices;
	std::vector<unsigned int> indices;
	std::vector<unsigned int> bone_indices_count;
	Bone bone;
	bool has_animation = false;
	bool has_bone = false;
	Material* material;
	std::string name;
	Shader* shader;
	Shader* shadow_shader;
	Shader* shadow_map_shader;

	void enable_attrib();
	void draw() const;
	std::pair<glm::vec3, glm::vec3> getBoundingBox() const;

private:
	VertexArray vertex_array;
	Buffer<Position> position_buffer;
	Buffer<Normal> normal_buffer;
	Buffer<Tangent> tangent_buffer;
	Buffer<BiTangent> bi_tangent_buffer;
	Buffer<TexCoord> tex_coord_buffer;
	Buffer<Color> color_buffer;
	Buffer<Weights> weights_buffer;
	Buffer<BoneIndices> bone_indices_buffer;
	ElementBuffer element_buffer;
};