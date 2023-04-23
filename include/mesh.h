#pragma once
#include <memory>
#include <climits>
#include "definition.h"
#include "vertex_attrib.h"
#include "material.h"
#include "buffer.h"
#include "bone.h"

class Object
{
public:
	virtual void draw(const Shader& shader, 
		const glm::mat4& view, 
		const glm::mat4& projection,
		const glm::mat4& model = glm::mat4(1.0f)
	) const = 0;
	virtual void draw(const Shader& shader, 
		unsigned int idx_animation,
		Microseconds time,
		const glm::mat4& view, 
		const glm::mat4& projection,
		const glm::mat4& model = glm::mat4(1.0f)
	) = 0;
	virtual std::pair<glm::vec3, glm::vec3> getBoundingBox() const = 0;
};

class Mesh: public Object
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
	std::vector<glm::mat4> transform;
	Bone bone;
	bool has_animation = false;
	bool has_bone = false;
	Material* material = nullptr;
	std::string name;

	void enable_attrib();

	void draw(
		const Shader& shader, 
		const glm::mat4& view,
		const glm::mat4& projection,
		const glm::mat4& model = glm::mat4(1.0f)
	) const override;

	void draw(const Shader& shader, 
		unsigned int idx_animation,
		Microseconds time,
		const glm::mat4& view, 
		const glm::mat4& projection,
		const glm::mat4& model = glm::mat4(1.0f)
	) override;

	std::pair<glm::vec3, glm::vec3> getBoundingBox() const override;

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