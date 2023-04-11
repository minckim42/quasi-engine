#include "texture.h"
#include "stb_image.h"
#include <string>
using namespace std::string_literals;

Texture::Texture()
{
	glCreateTextures(GL_TEXTURE_2D, 1, &id);
}

Texture::Texture(const std::filesystem::path& image_path, bool flip)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &id);
	load_image(image_path, flip);
}

Texture::~Texture()
{
	if (id)
		glDeleteTextures(1, &id);
}

Texture::Texture(Texture&& other):
	id(other.id), width(other.id), height(other.height), channel_size(other.channel_size)
{
}

Texture& Texture::operator=(Texture&& other)
{
	id = other.id;
	width = other.width;
	height = other.height;
	channel_size = other.channel_size;
	other.id = 0;
	return *this;
}

void Texture::load_image(const std::filesystem::path& image_path, bool flip)
{
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