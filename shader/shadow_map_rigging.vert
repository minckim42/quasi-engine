#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec3 bi_tangent;
layout (location = 4) in vec2 tex_coord;
layout (location = 5) in vec4 color;
layout (location = 6) in vec4 weight;
layout (location = 7) in uvec4 bone_idx;

out vec4 v_shadow_tex;
out vec3 v_normal;
out vec2 v_tex_coord;
out vec3 v_position;
out mat3 v_normal_space;

// model
layout (location = 11) uniform mat4 model;
layout (std140, binding = 1) uniform TransformBlock
{
	mat4 transform[150];
};

// camera
layout (location = 21) uniform mat4 view;
layout (location = 22) uniform mat4 projection;

mat4 get_bone_transform()
{
	return
		(transform[bone_idx.x] * weight.x) +
		(transform[bone_idx.y] * weight.y) +
		(transform[bone_idx.z] * weight.z) +
		(transform[bone_idx.w] * weight.w) ;
}

void main()
{
	mat4 model_transformed = model * get_bone_transform();
	gl_Position = 
		projection * 
		view * 
		model_transformed *
		vec4(position, 1.0f);
}