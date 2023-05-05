#include <string>
#include "utilities/definition.hpp"
#include "opengl/texture.hpp"
// #include "image_processor/stb_image.hpp"
// #include "image_processor/stb_image_write.hpp"

#include "image.hpp"

using namespace std::string_literals;


Texture::Texture(const std::filesystem::path& image_path, bool flip)
{
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

void Texture::allocate_rgba(int width, int height)
{
	this->width = width;
	this->height = height;
	glCreateTextures(GL_TEXTURE_2D, 1, &id);
	glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureStorage2D(id, 1, GL_RGBA8, width, height);
}

void Texture::allocate_depth(int width, int height)
{
	this->width = width;
	this->height = height;
	glCreateTextures(GL_TEXTURE_2D, 1, &id);
	glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(id, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTextureParameteri(id, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTextureStorage2D(id, 1, GL_DEPTH_COMPONENT32F, width, height);
}

void Texture::allocate_cube_map(int width, int height)
{
	this->width = width;
	this->height = height;
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &id);
	glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureStorage2D(id, 1, GL_RGBA8, width, height);
}

void Texture::load_image(const std::filesystem::path& image_path, bool flip)
{
	path = image_path.string();
	Image image(image_path, flip);

	allocate_rgba(image.get_width(), image.get_height());
	set_image(image.get_width(), image.get_height(), image.get_buffer());
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

void Texture::set_image(int width, int height, unsigned char* data)
{
	glTextureSubImage2D(id, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateTextureMipmap(id);
}

void Texture::set_cube_image(const std::array<Image, 6>& images)
{
	glTextureSubImage3D(id, 0, 0, 0, 0, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, images[0].get_buffer());
	glTextureSubImage3D(id, 0, 0, 0, 1, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, images[1].get_buffer());
	glTextureSubImage3D(id, 0, 0, 0, 2, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, images[2].get_buffer());
	glTextureSubImage3D(id, 0, 0, 0, 3, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, images[3].get_buffer());
	glTextureSubImage3D(id, 0, 0, 0, 4, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, images[4].get_buffer());
	glTextureSubImage3D(id, 0, 0, 0, 5, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, images[5].get_buffer());
	glGenerateTextureMipmap(id);
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
	Image image;
	image.set_buffer(width, height, buffer.data());
	image.save_as(path);
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

Texture Texture::create_default_texture()
{
	Texture default_texture;
	unsigned int data = 0xffffffff;
	default_texture.allocate_rgba(1, 1);
	default_texture.set_image(1, 1, reinterpret_cast<unsigned char*>(&data));
	return default_texture;
}

Texture Texture::create_default_normal_map()
{
	Texture default_normal_map;
	unsigned int data = 0xffff8080;
	default_normal_map.allocate_rgba(1, 1);
	default_normal_map.set_image(1, 1, reinterpret_cast<unsigned char*>(&data));
	return default_normal_map;
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
	return &(container[image_path.string()] = Texture(image_path, flip));
}

Texture::Ptr Texture::add_texture(const std::string& image_path, bool flip)
{
	auto it = container.find(image_path);
	if (it != container.end())
	{
		return &it->second;
	}
	return &(container[image_path] = Texture(image_path, flip));
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

Texture Texture::create_texture(const std::filesystem::path& image_path, bool flip)
{
	Texture texture;
	texture.load_image(image_path, flip);
	return texture;
}

Texture Texture::create_buffer(int w, int h)
{
	Texture texture;
	texture.allocate_rgba(w, h);
	return texture;
}

Texture Texture::create_depth_buffer(int w, int h)
{
	Texture texture;
	texture.allocate_depth(w, h);
	return texture;
}

Texture Texture::create_cube_map(const std::array<std::filesystem::path, 6>& image_path, bool flip)
{
	Texture texture;

	std::array<Image, 6> images;
	for (int i = 0; i < 6; ++i)
	{
		images[i].load(image_path[i], flip);
	}
	texture.allocate_cube_map(images[0].get_width(), images[0].get_height());
	texture.set_cube_image(images);
	return texture;
}
