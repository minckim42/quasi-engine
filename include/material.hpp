#pragma once
#include <memory>
#include "opengl/texture.hpp"

class Material final
{
public:
	using Ptr = Material*;
	using Container = std::map<std::string, Material>;


	// static Ptr default_material();
	static Ptr add_empty_material(const std::string& name);
	static Ptr add_material(const Material& other);
	static Ptr get_material(const std::string& name);
	static bool delete_material(const std::string& name);

	Texture::Ptr ambient;
	Texture::Ptr diffuse;
	Texture::Ptr specular;
	Texture::Ptr normal;
	glm::vec3 ka = {0.0f, 0.0f, 0.0f};
	glm::vec3 kd = {0.0f, 0.0f, 0.0f};
	glm::vec3 ks = {0.0f, 0.0f, 0.0f};
	glm::vec3 tr = {0.0f, 0.0f, 0.0f}; // transparent
	std::string name;

	void bind(const Shader& shader) const noexcept;

private:
	inline static Container container;
};