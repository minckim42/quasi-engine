#include <string>
#include "utilities/definition.hpp"
#include "opengl/texture.hpp"
#include "image_processor/stb_image.hpp"
#include "image_processor/stb_image_write.hpp"

using namespace std::string_literals;

Texture::Texture(): id(0)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &id);
}

Texture::Texture(const std::filesystem::path& image_path, bool flip):
	path(image_path.string())
{
	glCreateTextures(GL_TEXTURE_2D, 1, &id);
	load_image(image_path, flip);
}

Texture::~Texture()
{
	if (id)
		glDeleteTextures(1, &id);
}

Texture::Texture(Texture&& other) noexcept:
	id(other.id), width(other.id), height(other.height), channel_size(other.channel_size), path(other.path)
{
	other.id = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept
{
	if (this == &other) return *this;
	id = other.id;
	width = other.width;
	height = other.height;
	channel_size = other.channel_size;
	path = other.path;
	other.id = 0;
	return *this;
}

void Texture::allocate(int width, int height, Texture::FORMAT format)
{
	this->width = width;
	this->height = height;
	this->format = format;
	if (format == FORMAT::RGBA8)
	{
		glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureStorage2D(id, 1, GL_RGBA8, width, height);

	}
	else if (format == FORMAT::DEPTH_COMPONENT)
	{
		glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(id, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTextureParameteri(id, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTextureStorage2D(id, 1, GL_DEPTH_COMPONENT32F, width, height);
	}
}

void Texture::load_image(const std::filesystem::path& image_path, bool flip)
{
	format = FORMAT::RGBA8;
	glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(flip);
	int dummy_channel_size = 0;
	unsigned char* data = 
		stbi_load(
			(root_path / image_path).string().data(), 
			&width, 
			&height, 
			&dummy_channel_size, 
			4
		);
	if (!data)
		throw std::runtime_error("[Error] Failed to open image: "s + image_path.string());

	glTextureStorage2D(id, 1, GL_RGBA8, width, height);
	glTextureSubImage2D(id, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateTextureMipmap(id);
	stbi_image_free(data);
}

void Texture::bind(const Shader& shader, const std::string_view& name, GLuint unit) const
{
	glBindTextureUnit(unit, id);
	shader.set(name, unit);
}

void Texture::bind(const Shader& shader, GLuint location, GLuint unit) const
{
	glBindTextureUnit(unit, id);
	shader.set(location, unit);
}

std::vector<unsigned char> Texture::get_image() const
{
	std::vector<unsigned char> buffer(get_buffer_size());
	glGetTextureImage(id, 0, GL_RGBA, GL_UNSIGNED_BYTE, get_buffer_size(), buffer.data());
	return buffer;
}

void Texture::save_as_image(const std::filesystem::path& path) const
{
	std::vector<unsigned char> buffer = get_image();
	std::string extension = path.extension().string();
	stbi_flip_vertically_on_write(true);
	if (extension == ".png")
	{
		stbi_write_png(path.string().data(), width, height, channel_size, buffer.data(), 0);
	}
	else if (extension == ".jpg")
	{
		stbi_write_jpg(path.string().data(), width, height, channel_size, buffer.data(), 90);
	}

}

size_t Texture::get_buffer_size() const
{
	return width * height * channel_size;
}

Texture::operator GLuint() const
{
	return id;
}

Texture::operator GLuint()
{
	return id;
}

Texture Texture::create_default_normal_map()
{
	Texture default_normal_map;
	glTextureParameteri(default_normal_map, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(default_normal_map, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(default_normal_map, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(default_normal_map, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned int data = 0xffff8080;
	glTextureStorage2D(default_normal_map, 1, GL_RGBA8, 1, 1);
	glTextureSubImage2D(default_normal_map, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &data);
	glGenerateTextureMipmap(default_normal_map);
	return default_normal_map;
}

Texture Texture::create_default_texture()
{
	Texture default_texture;
	glTextureParameteri(default_texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(default_texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(default_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(default_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned int data = 0xffffffff;
	glTextureStorage2D(default_texture, 1, GL_RGBA8, 1, 1);
	glTextureSubImage2D(default_texture, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &data);
	glGenerateTextureMipmap(default_texture);
	return default_texture;
}

Texture::Ptr Texture::default_texture()
{
	static Texture default_texture = create_default_texture();
	return &default_texture;
}

Texture::Ptr Texture::default_normal_map()
{
	static Texture default_normal_map = create_default_normal_map();
	return &default_normal_map;
}

Texture::Ptr Texture::add_texture(const std::filesystem::path& image_path, bool flip)
{
	auto it = container.find(image_path.string());
	if (it != container.end())
	{
		return &it->second;
	}
	container[image_path.string()] = Texture(image_path, flip);
}

Texture::Ptr Texture::add_texture(const std::string& image_path, bool flip)
{
	auto it = container.find(image_path);
	if (it != container.end())
	{
		return &it->second;
	}
	container[image_path] = Texture(image_path, flip);
}

Texture::Ptr Texture::get_texture(const std::string& name)
{
	auto it = container.find(name);
	if (it == container.end())
	{
		return nullptr;
	}
	return &it->second;
}

bool Texture::delete_texture(const std::string& name)
{
	auto it = container.find(name);
	if (it == container.end())
	{
		return false;
	}
	container.erase(it);
	return true;
}