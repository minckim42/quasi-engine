#include "bone.h"
#include "model.h"

Bone::Bone(const std::shared_ptr<Model> model):
	name(model->name), transform(model->transform)
{
	children.reserve(model->children.size());
	for (const std::shared_ptr<Model> node_child: model->children)
	{
		children.emplace_back(node_child);
	}
}

Bone* Bone::find(const std::string& name)
{
	if (name == this->name)
	{
		return this;
	}
	Bone* result = nullptr;
	for (Bone& child: children)
	{
		result = child.find(name);
		if (result)
			return result;
	}
	return nullptr;
}

void Bone::update(
	std::vector<glm::mat4>& transforms, 
	unsigned int idx_animation, 
	Microseconds time, 
	glm::mat4 parent_transform
) const
{
	if (!animations.empty())
	{
		parent_transform = parent_transform * animations[idx_animation].interpolate(time);
		if (idx != UINT_MAX)
		{
			transforms[idx] = parent_transform * offset;
		}
	}
	for (const Bone& child: children)
	{
		child.update(transforms, idx_animation, time, parent_transform);
	}
	return;
}

