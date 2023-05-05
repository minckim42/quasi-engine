#pragma once
#include <glad/glad.h>
#include <filesystem>
#include <string_view>
#include <map>
#include "shader.hpp"
#include "image.hpp"

class Texture
{
public:
	using Container = std::map<std::string, Texture>;
	using Ptr = Texture*;

	enum class FORMAT
	{
		NONE,
		RGBA,
		DEPTH,
		CUBE_MAP,
	};

	Texture() = default;
	static Ptr default_texture();
	static Ptr default_normal_map();
	static Ptr add_texture(const std::filesystem::path& image_path, bool flip = true);
	static Ptr add_texture(const std::string& image_path, bool flip = true);
	static Ptr get_texture(const std::string& name);
	static bool delete_texture(const std::string& name);

	static Texture create_texture(const std::filesystem::path& image_path, bool flip = true);
	static Texture create_buffer(int w, int h);
	static Texture create_depth_buffer(int w, int h);
	static Texture create_cube_map(const std::array<std::filesystem::path, 6>& image_path, bool flip = false);

	Texture(Texture&& other) noexcept;
	Texture& operator=(Texture&& other) noexcept;
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	~Texture();

	void allocate_rgba(int width, int height);
	void allocate_depth(int width, int height);
	void allocate_cube_map(int width, int height);
	void load_image(const std::filesystem::path& image_path, bool flip = true);
	void bind(const Shader& shader, const std::string_view& name, GLuint unit) const;
	void bind(const Shader& shader, GLuint location, GLuint unit) const;
	void set_image(int width, int height, unsigned char* data);
	void set_cube_image(const std::array<Image, 6>& images);
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
	std::string path;

	static Texture create_default_normal_map();
	static Texture create_default_texture();

	explicit Texture(const std::filesystem::path& image_path, bool flip = true);
};
