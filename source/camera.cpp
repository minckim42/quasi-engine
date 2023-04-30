#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include "camera.hpp"

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

void Camera::set_uniform(const Shader& shader) const noexcept
{
	shader.set(static_cast<int>(Shader::UNIFORM_LOCATION::EYE), position);
	shader.set(static_cast<int>(Shader::UNIFORM_LOCATION::VIEW), view);
	shader.set(static_cast<int>(Shader::UNIFORM_LOCATION::PROJECTION), projection);
}

std::ostream& operator<<(std::ostream& os, Camera& camera)
{
	return os << "[Camera]\n"
		<< " position: " << glm::to_string(camera.position) << '\n'
		<< " direction: " << glm::to_string(camera.direction) << '\n'
		<< " up: " << glm::to_string(camera.up) << '\n'
		<< " right: " << glm::to_string(camera.right) << '\n';
}
