#pragma once
#include <memory>
#include "texture.h"

struct Material
{
	Texture* ambient = nullptr;
	Texture* diffuse = nullptr;
	Texture* specular = nullptr;
	Texture* normal = nullptr;

	glm::vec3 ka = {0.0f, 0.0f, 0.0f};
	glm::vec3 kd = {0.0f, 0.0f, 0.0f};
	glm::vec3 ks = {0.0f, 0.0f, 0.0f};
	using Container = std::map<std::string, Material>;
	inline static std::map<std::string, Material> container;

	void bind(const Shader& shader) const noexcept;
};