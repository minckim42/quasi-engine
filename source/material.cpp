#include "material.h"

void Material::bind(const Shader& shader) const noexcept
{
	if (ambient)
	{
		ambient->bind(shader, 0, 0);
	}
	if (diffuse)
	{
		diffuse->bind(shader, "diffuse", 1);
	}
	if (specular)
	{
		specular->bind(shader, 2, 2);
	}
	if (normal)
	{
		normal->bind(shader, 3, 3);
	}
	shader.set("ka", ka);
	shader.set("kd", kd);
	shader.set("ks", ks);
}