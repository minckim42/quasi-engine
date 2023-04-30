#version 460 core

layout (location = 0) in vec3 position;

// model
layout (location = 11) uniform mat4 model;

// camera
layout (location = 21) uniform mat4 view;
layout (location = 22) uniform mat4 projection;

out vec3 direction;

const mat3 tr = mat3(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	direction = tr * position;
}