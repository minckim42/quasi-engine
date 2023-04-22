#version 460 core

layout (binding = 1) uniform sampler2D tex;
in vec3 v_normal;
in vec2 v_tex_coord;
out vec4 FragColor;

void main()
{
	FragColor = texture(tex, v_tex_coord);
}