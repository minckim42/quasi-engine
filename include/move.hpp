#pragma once
#include <glm/glm.hpp>

class Move
{
public:
	glm::vec3 position = {0.0f,0.0f,0.0f};
	glm::vec3 direction = {0.0f, 1.0f, 0.0f};
	glm::vec3 up = {0.0f, 0.0f, 1.0f};
	glm::vec3 right = {1.0f, 0.0f, 0.0f};

	Move() = default;
	Move(glm::vec3 position, glm::vec3 direction, glm::vec3 up);

	void move_forward(float val) noexcept;
	void move_backward(float val) noexcept;
	void move_left(float val) noexcept;
	void move_right(float val) noexcept;
	void move_up(float val) noexcept;
	void move_down(float val) noexcept;
	void pitch_up(float rad) noexcept;
	void pitch_down(float rad) noexcept;
	void turn_left(float val) noexcept;
	void turn_right(float val) noexcept;

	void update_right() noexcept;
};

glm::vec3 rotate(const glm::vec3& input, glm::vec3 axis, float rad) noexcept;