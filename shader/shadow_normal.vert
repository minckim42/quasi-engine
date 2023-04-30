#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec3 bi_tangent;
layout (location = 4) in vec2 tex_coord;

out vec4 v_shadow_tex;
out vec3 v_normal;
out vec2 v_tex_coord;
out vec3 v_position;
out mat3 v_normal_space;

// model
layout (location = 11) uniform mat4 model;

// camera
layout (location = 21) uniform mat4 view;
layout (location = 22) uniform mat4 projection;

// shadow
layout (location = 31) uniform mat4 shadow_view;
layout (location = 32) uniform mat4 shadow_projection;

const mat4 offset_tex = mat4(
	0.5f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.5f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.5f, 0.0f,
	0.5f, 0.5f, 0.5f, 1.0f
);

void main()
{
	gl_Position = 
		projection * 
		view * 
		model *
		vec4(position, 1.0f);
	v_tex_coord = tex_coord;
	v_normal = normalize((model * vec4(normal, 0.0f))).xyz;
	v_position = (model * vec4(position, 1.0f)).xyz;
	v_normal_space = (mat3(
		normalize((model * vec4(bi_tangent, 0.0f))).xyz,
		normalize((model * vec4(tangent, 0.0f))).xyz,
		normalize((model * vec4(normal, 0.0f))).xyz
	));

	v_shadow_tex = offset_tex * shadow_projection * shadow_view * model * vec4(position, 1.0f);
}