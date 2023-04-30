#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include "animation.hpp"
#include "utilities/binary_search.hpp"

static glm::mat4 interpolate_position(const Position& a, const Position& b, float delta)
{
	return {
		{1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f, 0.0f},
		glm::vec4(a * (1.0f - delta) + b * delta, 1.0f)
	};
}

static glm::mat4 interpolate_scale(const Scaling& a, const Scaling& b, float delta)
{
	glm::vec3 scale(a * (1.0f - delta) + b * delta);
	return {
		{scale.x, 0.0f, 0.0f, 0.0f},
		{0.0f, scale.y, 0.0f, 0.0f},
		{0.0f, 0.0f, scale.z, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f},
	};
}

static glm::mat4 interpolate_rotation(const Rotation& a, const Rotation& b, float delta)
{
	if (delta == 0) return glm::mat4(a);
	return glm::mat4(glm::slerp(a, b, delta));
}

template <typename KeyType, typename InterpolateFunction>
static glm::mat4 interpolate_key(const std::vector<KeyType>& keys, Microseconds time, InterpolateFunction interpolate_fucntion)
{
	if (keys.empty()) return glm::mat4(1.0f);
	KeyType key_to_find = {time};
	auto left = b_search(keys.begin(), keys.end(), key_to_find, 
		[](const KeyType& l, const KeyType& r){
			return l.time < r.time;
		}
	);
	auto right = std::min(keys.end() - 1, left + 1);
	float delta;
	if (left->time == right->time)
		delta = 0.0f;
	else
		delta = static_cast<float>((time - left->time).count()) / (right->time - left->time).count();
	return interpolate_fucntion(left->key, right->key, delta);
}

glm::mat4 Animation::interpolate(Microseconds time) const
{
	using namespace std::chrono_literals;
	
	return 
		interpolate_key(position_key, time, interpolate_position) *
		interpolate_key(rotation_key, time, interpolate_rotation) *
		interpolate_key(scaling_key, time, interpolate_scale) *
		glm::mat4(1.0f);
}