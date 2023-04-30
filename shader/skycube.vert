#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

// model
layout (location = 11) uniform mat4 model;

// camera
layout (location = 21) uniform mat4 view;
layout (location = 22) uniform mat4 projection;

out vec3 v_normal;
out vec3 v_position;

const mat3 tr = mat3(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	v_normal = (model * vec4(position, 0.0f)).xyz;
	v_position = (model * vec4(position, 1.0f)).xyz;
}