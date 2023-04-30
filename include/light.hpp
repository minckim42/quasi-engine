#pragma once
#include "vertex_attrib.hpp"

class PointLight
{
public:
	Position position;
	float padding;
	Color color = {1.0f, 1.0f, 1.0f, 1.0f};
	float intensity;
	float padding2[3];

private:
};