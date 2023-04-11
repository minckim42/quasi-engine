#pragma once
#include <glad/glad.h>
#include <filesystem>
#include <string_view>
#include "shader.h"

class Texture
{
public:
	Texture();
	Texture(const std::filesystem::path& image_path, bool flip = true);
	~Texture();
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	Texture(Texture&& other);
	Texture& operator=(Texture&& other);

	void load_image(const std::filesystem::path& image_path, bool flip = true);
	void bind(const Shader& shader, const std::string_view& name, GLuint unit) const;

private:
	GLuint id = 0;
	int width = 0;
	int height = 0;
	int channel_size = 4;

	inline static const std::filesystem::path root_path = ROOT;
};