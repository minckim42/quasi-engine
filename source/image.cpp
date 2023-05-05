#include "image.hpp"
#include "image_processor/stb_image.hpp"
#include "image_processor/stb_image_write.hpp"
#include <stdexcept>
#include <string>

Image::Image(const std::filesystem::path& path, bool flip): data(nullptr)
{
	load(path, flip);
}

void Image::load(const std::filesystem::path& path, bool flip)
{
	stbi_set_flip_vertically_on_load(flip);
	using namespace std::string_literals;
	channel_size = 4;
	int dummy_channel_size;
	data = stbi_load(path.string().data(), &width, &height, &dummy_channel_size, 4);
	if (!data)
		throw std::runtime_error("[Error] Failed to open image: "s + path.string());
}

Image::~Image()
{
	if (data)
		stbi_image_free(data);
}

void Image::save_as(const std::filesystem::path& path, bool flip) const
{
	std::string extension = path.extension().string();
	stbi_flip_vertically_on_write(flip);
	if (extension == ".png")
	{
		stbi_write_png(path.string().data(), width, height, channel_size, data, 0);
	}
	else if (extension == ".jpg")
	{
		stbi_write_jpg(path.string().data(), width, height, channel_size, data, 90);
	}
}

void Image::set_buffer(int width, int height, unsigned char* buffer)
{
	data = buffer;
	this->width = width;
	this->height = height;
}

unsigned char* Image::get_buffer() const
{
	return data;
}

int Image::get_width() const
{
	return width;
}

int Image::get_height() const
{
	return height;
}
