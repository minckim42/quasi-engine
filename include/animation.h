#pragma once
#include <vector>
#include <chrono>
#include "vertex_attrib.h"
#include "definition.h"

template <typename Type>
struct Key
{
	Microseconds time;
	Type key;
};

using PositionKey = Key<Position>;
using RotationKey = Key<Rotation>;
using ScalingKey = Key<Scaling>;

struct Animation
{
	std::vector<PositionKey> position_key;
	std::vector<RotationKey> rotation_key;
	std::vector<ScalingKey> scaling_key;

	glm::mat4 interpolate(Microseconds time) const;
};