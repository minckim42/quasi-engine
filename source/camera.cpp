#include "camera.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

Camera::Camera(
	const glm::vec3& position,
	const glm::vec3& direction,
	const glm::vec3& up,
	const glm::vec2& screen_size,
	float near,
	float far
) :
	Move(position, direction, up),
	screen_size(screen_size),
	near(near),
	far(far)
{
	fovy = glm::pi<float>() * 0.3333333f;
	update_all();
}

void Camera::update_all() noexcept
{
	update_view();
	update_projection();
}

void Camera::update_view() noexcept
{
	view = glm::lookAt(position, position + direction, up);
}
void Camera::update_projection() noexcept
{
	projection = glm::perspective(fovy, screen_size.x / screen_size.y, near, far);
}

std::ostream& operator<<(std::ostream& os, Camera& camera)
{
	return os << "[Camera]\n"
		<< " position: " << glm::to_string(camera.position) << '\n'
		<< " direction: " << glm::to_string(camera.direction) << '\n'
		<< " up: " << glm::to_string(camera.up) << '\n'
		<< " right: " << glm::to_string(camera.right) << '\n';
}