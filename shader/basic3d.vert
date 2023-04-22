#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec3 bi_tangent;
layout (location = 4) in vec2 tex_coord;
layout (location = 5) in vec4 color;
layout (location = 6) in vec4 weight;
layout (location = 7) in uvec4 bone_idx;

out vec3 v_normal;
out vec2 v_tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 transform[150];

mat4 get_model()
{
	return
		(transform[bone_idx.x] * weight.x) +
		(transform[bone_idx.y] * weight.y) +
		(transform[bone_idx.z] * weight.z) +
		(transform[bone_idx.w] * weight.w) ;
}

void main()
{
	gl_Position = 
		projection * 
		view * 
		model *
		get_model() * 
		vec4(position, 1.0f);
	v_normal = normal;
	v_tex_coord = tex_coord;
}