#include "move.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <cmath>

Move::Move(glm::vec3 position, glm::vec3 direction, glm::vec3 up):
position(position),
direction(glm::normalize(direction)),
up(glm::normalize(up)),
right(glm::normalize(glm::cross(direction, up)))
{}

void Move::move_forward(float val) noexcept
{
	position += direction * val;
}

void Move::move_backward(float val) noexcept
{
	position -= direction * val;
}

void Move::move_left(float val) noexcept
{
	position -= right * val;
}

void Move::move_right(float val) noexcept
{
	position += right * val;
}

void Move::move_up(float val) noexcept
{
	position += up * val;
}

void Move::move_down(float val) noexcept
{
	position -= up * val;
}

void Move::pitch_up(float rad) noexcept
{
	if (cosf(rad) < glm::dot(direction, up)) return;
	direction = rotate(direction, right, rad);
}

void Move::pitch_down(float rad) noexcept
{
	if (-cosf(rad) > glm::dot(direction, up)) return;
	direction = rotate(direction, right, -rad);
}

void Move::turn_left(float rad) noexcept
{
	//right = rotate(right, up, rad);
	direction = rotate(direction, up, rad);
	update_right();
}

void Move::turn_right(float rad) noexcept
{
	//right = rotate(right, up, -rad);
	direction = rotate(direction, up, -rad);
	update_right();
}

void Move::update_right() noexcept
{
	right = glm::cross(direction, up);
	right = glm::normalize(right);
}

glm::vec3 rotate(const glm::vec3& input, glm::vec3 axis, float rad) noexcept
{
	rad *= 0.5f;
	return glm::quat(std::cos(rad), axis * std::sin(rad)) * input;
}