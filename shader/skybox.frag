#version 460 core

layout (location = 1, binding = 1) uniform samplerCube cubemap;

out vec4 FragColor;

in vec3 direction;

const float zero = 0.0f;
const float one = 1.0f;

void main()
{
	FragColor = texture(cubemap, direction);
	// FragColor = vec4(1, 1, 0, 1);
}