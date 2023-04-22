#version 460 core

layout (binding = 0) uniform sampler2D ambient;
layout (binding = 1) uniform sampler2D diffuse;
layout (binding = 2) uniform sampler2D specular;
layout (binding = 3) uniform sampler2D normal;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;

in vec3 v_color;
in vec2 v_tex_coord;
out vec4 FragColor;

void main()
{
	FragColor = 
		texture(ambient, v_tex_coord) * vec4(ka, 1.0f)  * 1.0f +
		texture(diffuse, v_tex_coord) * vec4(kd, 1.0f)  * 1.0f +
		texture(specular, v_tex_coord) * vec4(ks, 1.0f) * 1.0f ;
}