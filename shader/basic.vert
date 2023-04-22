#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 4) in vec2 tex_coord;

out vec3 v_normal;
out vec2 v_tex_coord;

void main()
{
	gl_Position = vec4(position, 1.0f);
	v_normal = normal;
	v_tex_coord = tex_coord;
}