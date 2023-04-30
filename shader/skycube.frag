#version 460 core

layout (location = 1, binding = 1) uniform samplerCube cubemap;

out vec4 FragColor;

in vec3 v_normal;
in vec3 v_position;

layout (location = 52) uniform vec3 eye;

const float zero = 0.0f;
const float one = 1.0f;
const mat3 tr = mat3(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

void main()
{
	vec3 direction = tr * reflect(v_position - eye, normalize(v_normal));
	FragColor = texture(cubemap, direction);
	// FragColor = vec4(1, 1, 0, 1);
}