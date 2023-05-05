#pragma once
#include <filesystem>

class Image
{
public:
	Image() = default;
	Image(const std::filesystem::path& path, bool flip = true);
	~Image();

	void load(const std::filesystem::path& path, bool flip = true);
	void save_as(const std::filesystem::path& path, bool flip = true) const;
	void set_buffer(int width, int height, unsigned char* data);
	unsigned char* get_buffer() const;
	int get_width() const;
	int get_height() const;

private:
	int width = 0;
	int height = 0;
	int channel_size = 4;
	unsigned char* data = nullptr;
};