#version 460 core

layout (location = 1, binding = 1) uniform sampler2D ambient;
layout (location = 2, binding = 2) uniform sampler2D diffuse;
layout (location = 3, binding = 3) uniform sampler2D specular;
layout (location = 4, binding = 4) uniform sampler2D normal_map;

out vec4 FragColor;

in vec3 v_color;
in vec2 v_tex_coord;

layout (location = 41) uniform vec3 ka;
layout (location = 42) uniform vec3 kd;
layout (location = 43) uniform vec3 ks;


struct LightStruct 
{
	vec4 position;
	vec4 color;
	float intensity;
};

layout (std140, binding = 2) uniform LightBlock
{
	LightStruct lights[100];
};
layout (location = 51) uniform uint n_lights;

vec4 apply_diffuse(int i)
{
	return lights[i].color * lights[i].intensity * lights[i].position / 1000.0f;
}

void main()
{
	// FragColor = blocks[0].color + blocks[1].color + blocks[2].color;
	FragColor = vec4(0.0f);

	for (int i = 0; i < n_lights; ++i)
	{
		FragColor += apply_diffuse(i);
	}
	FragColor.a = 1.0f;

}