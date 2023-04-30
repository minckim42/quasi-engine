#pragma once
#include "texture.hpp"
class Framebuffer
{
public:
	Framebuffer();

	Framebuffer(const Texture& texture);
	~Framebuffer();
	Framebuffer(const Framebuffer&) = delete;
	Framebuffer& operator=(const Framebuffer&) = delete;

	Framebuffer(Framebuffer&& other) noexcept;
	Framebuffer& operator=(Framebuffer&& other) noexcept;

	void bind() const;
	void attach_colorbuffer(const Texture& texture) const;
	void attach_depthbuffer(const Texture& texture) const;
	bool check_status() const;
	operator unsigned int() const;
	operator unsigned int();

	static void bind_default();

private:
	unsigned int id;
};