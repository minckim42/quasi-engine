#pragma once
#include <vector>
#include "mesh.hpp"

class Model
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

	std::pair<glm::vec3, glm::vec3> getBoundingBox() const;

	bool cull_empty();
};
