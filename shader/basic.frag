#version 460 core

layout (binding = 1) uniform sampler2D tex;
in vec3 v_color;
in vec2 v_tex_coord;
out vec4 FragColor;

void main()
{
	// FragColor = vec4(v_color, 1.0f);
	FragColor = texture(tex, v_tex_coord);
}