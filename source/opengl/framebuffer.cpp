#include "opengl/framebuffer.hpp"
#include <glad/glad.h>

Framebuffer::Framebuffer(): id(0)
{
	glCreateFramebuffers(1, &id);
}

Framebuffer::Framebuffer(const Texture& texture)
{
	glCreateFramebuffers(1, &id);
	attach_colorbuffer(texture);
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept: id(other.id)
{
	other.id = 0;
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
{
	if (this == &other) return *this;
	id = other.id;
	other.id = 0;
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &id);
}

void Framebuffer::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void Framebuffer::attach_colorbuffer(const Texture& texture) const
{
	glNamedFramebufferTexture(id, GL_COLOR_ATTACHMENT0, texture, 0);
}

void Framebuffer::attach_depthbuffer(const Texture& texture) const
{
	glNamedFramebufferTexture(id, GL_DEPTH_ATTACHMENT, texture, 0);

}

Framebuffer::operator unsigned int() const
{
	return id;
}

Framebuffer::operator unsigned int()
{
	return id;
}

bool Framebuffer::check_status() const
{
	return glCheckNamedFramebufferStatus(id, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void Framebuffer::bind_default()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}