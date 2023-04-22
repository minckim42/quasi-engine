#pragma once
#include <vector>
#include <chrono>
#include <memory>
#include "animation.h"

class Model;

struct Bone
{
	unsigned int idx = UINT_MAX;
	std::string name;
	glm::mat4 transform = glm::mat4(1.0f);
	glm::mat4 offset = glm::mat4(1.0f);
	std::vector<Bone> children;
	std::vector<Animation> animations;

	Bone() = default;
	Bone(const std::shared_ptr<Model> model);
	Bone(Bone&& bone) noexcept = default;
	Bone& operator=(Bone&& bone) noexcept = default;

	Bone* find(const std::string& name);
	void update(
		std::vector<glm::mat4>& transform, 
		Microseconds time,
		glm::mat4 parent_transform = glm::mat4(1.0f) 
	) const;
	void update(
		std::vector<glm::mat4>& transform, 
		unsigned int idx_animation,
		Microseconds time,
		glm::mat4 parent_transform = glm::mat4(1.0f) 
	) const;
};