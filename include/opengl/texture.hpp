#pragma once
#include <glad/glad.h>
#include <filesystem>
#include <string_view>
#include <map>
#include "shader.hpp"

class Texture
{
public:
	using Container = std::map<std::string, Texture>;
	using Ptr = Texture*;

	enum class FORMAT
	{
		RGBA8 = GL_RGBA8,
		DEPTH_COMPONENT = GL_DEPTH_COMPONENT32F,
	};

	static Ptr default_texture();
	static Ptr default_normal_map();
	static Ptr add_texture(const std::filesystem::path& image_path, bool flip = true);
	static Ptr add_texture(const std::string& image_path, bool flip = true);
	static Ptr get_texture(const std::string& name);
	static bool delete_texture(const std::string& name);

	Texture();
	explicit Texture(const std::filesystem::path& image_path, bool flip = true);
	Texture(Texture&& other) noexcept;
	Texture& operator=(Texture&& other) noexcept;
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	~Texture();

	void allocate(int width, int height, FORMAT format = FORMAT::RGBA8);
	void load_image(const std::filesystem::path& image_path, bool flip = true);
	void bind(const Shader& shader, const std::string_view& name, GLuint unit) const;
	void bind(const Shader& shader, GLuint location, GLuint unit) const;
	std::vector<unsigned char> get_image() const;
	void save_as_image(const std::filesystem::path& path) const;
	size_t get_buffer_size() const;
	operator GLuint() const;
	operator GLuint();

private:
	inline static Container container;
	
	GLuint id = 0;
	int width = 0;
	int height = 0;
	int channel_size = 4;
	FORMAT format;
	std::string path;

	static Texture create_default_normal_map();
	static Texture create_default_texture();
};