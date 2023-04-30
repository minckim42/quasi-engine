#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec3 bi_tangent;
layout (location = 4) in vec2 tex_coord;
layout (location = 5) in vec4 color;

// model
layout (location = 11) uniform mat4 model;

// camera
layout (location = 21) uniform mat4 view;
layout (location = 22) uniform mat4 projection;

void main()
{
	gl_Position = 
		projection * 
		view * 
		model *
		vec4(position, 1.0f);
}