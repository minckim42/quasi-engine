#include "model.hpp"

std::pair<glm::vec3, glm::vec3> Model::get_bounding_box() const
{
	constexpr std::pair<glm::vec3, glm::vec3> empty_case = {
		{FLT_MAX, FLT_MAX, FLT_MAX},
		{-FLT_MAX, -FLT_MAX, -FLT_MAX}
	};
	std::pair<glm::vec3, glm::vec3> result = {
		{FLT_MAX, FLT_MAX, FLT_MAX},
		{-FLT_MAX, -FLT_MAX, -FLT_MAX}
	};
	for (const auto& mesh: meshes)
	{
		auto bounding_box = mesh->get_bounding_box();
		result.first = glm::min(result.first, bounding_box.first);
		result.second = glm::max(result.second, bounding_box.second);
	}
	for (const auto& child: children)
	{
		auto bounding_box = child->get_bounding_box();
		result.first = glm::min(result.first, bounding_box.first);
		result.second = glm::max(result.second, bounding_box.second);
	}
	if (result == empty_case)
		return result;
	result.first = glm::vec3(transform * glm::vec4(result.first, 1.0f));
	result.second = glm::vec3(transform * glm::vec4(result.second, 1.0f));
	return result;
}

bool Model::cull_empty()
{
	auto it = children.begin();
	while (it < children.end())
	{
		if ((*it)->cull_empty())
			it = children.erase(it);
		else
			++it;
	}
	if (children.empty() && meshes.empty())
	{
		return true;
	}
	return false;
}