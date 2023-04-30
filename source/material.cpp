#include "material.hpp"


#include <iostream>
void Material::bind(const Shader& shader) const noexcept
{
	if (ambient)
	{
		ambient->bind(
			shader, 
			static_cast<int>(Shader::TEXTURE_LOCATION_BINDING::AMBIENT), 
			static_cast<int>(Shader::TEXTURE_LOCATION_BINDING::AMBIENT)
		);
	}
	if (diffuse)
	{
		diffuse->bind(
			shader, 
			static_cast<int>(Shader::TEXTURE_LOCATION_BINDING::DIFFUSE), 
			static_cast<int>(Shader::TEXTURE_LOCATION_BINDING::DIFFUSE)
		);
	}
	if (specular)
	{
		specular->bind(
			shader, 
			static_cast<int>(Shader::TEXTURE_LOCATION_BINDING::SPECULAR), 
			static_cast<int>(Shader::TEXTURE_LOCATION_BINDING::SPECULAR)
		);
	}
	if (normal)
	{
		normal->bind(
			shader, 
			static_cast<int>(Shader::TEXTURE_LOCATION_BINDING::NORMAL_MAP), 
			static_cast<int>(Shader::TEXTURE_LOCATION_BINDING::NORMAL_MAP)
		);
	}
	shader.set(static_cast<int>(Shader::UNIFORM_LOCATION::KA), ka);
	shader.set(static_cast<int>(Shader::UNIFORM_LOCATION::KD), kd);
	shader.set(static_cast<int>(Shader::UNIFORM_LOCATION::KS), ks);
}

Material::Ptr Material::add_empty_material(const std::string& name)
{
	return &(container[name] = Material());
}

Material::Ptr Material::add_material(const Material& other)
{
	auto it = container.find(other.name);
	if (it != container.end())
	{
		return &it->second;
	}
	return &(container[other.name] = other);
}

Material::Ptr Material::get_material(const std::string& name)
{
	auto it = container.find(name);
	if (it == container.end())
	{
		return nullptr;
	}
	return &it->second;
}

bool Material::delete_material(const std::string& name)
{
	auto it = container.find(name);
	if (it == container.end())
	{
		return false;
	}
	container.erase(it);
	return true;
}