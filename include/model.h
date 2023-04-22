#pragma once
#include <vector>
#include "mesh.h"

class Model: public Object
{
public:
	std::string name;
	glm::mat4 transform = glm::mat4(1.0f);
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<Model>> children;

	Model() = default;
	Model(const Model&) = delete;
	Model(Model&& other) noexcept = default;
	Model& operator=(const Model&) = delete;
	Model& operator=(Model&& other) noexcept = default;

	void draw(
		const Shader& shader, 
		const glm::mat4& view,
		const glm::mat4& projection,
		const glm::mat4& model = glm::mat4(1.0f)
	) const override;
	virtual void draw(
		const Shader& shader, 
		unsigned int idx_animation,
		Microseconds time,
		const glm::mat4& view, 
		const glm::mat4& projection,
		const glm::mat4& model = glm::mat4(1.0f)
	) override;
	std::pair<glm::vec3, glm::vec3> getBoundingBox() const override;
};