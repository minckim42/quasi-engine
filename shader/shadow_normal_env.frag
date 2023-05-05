#version 460 core

layout (location = 1, binding = 1) uniform sampler2D ambient;
layout (location = 2, binding = 2) uniform sampler2D diffuse;
layout (location = 3, binding = 3) uniform sampler2D specular;
layout (location = 4, binding = 4) uniform sampler2D normal_map;
layout (location = 9, binding = 9) uniform samplerCube cubemap;
layout (location = 10, binding = 10) uniform sampler2DShadow shadow_map;

out vec4 FragColor;

in vec4 v_shadow_tex;
in vec3 v_normal;
in vec2 v_tex_coord;
in vec3 v_position;
in mat3 v_normal_space;

layout (location = 41) uniform vec3 ka;
layout (location = 42) uniform vec3 kd;
layout (location = 43) uniform vec3 ks;
layout (location = 44) uniform vec3 tr;
layout (location = 45) uniform vec3 bump_scaling;

struct LightStruct 
{
	vec3 position;
	vec4 color;
	float intensity;
};
layout (std140, binding = 2) uniform LightBlock
{
	LightStruct lights[100];
};

layout (location = 51) uniform uint n_lights;
layout (location = 52) uniform vec3 eye;


const float zero = 0.0f;
const float one = 1.0f;
const float min_diffuse = 0.4f;
const vec3 normal_scaling = vec3(1.0f, 0.3f, 0.3f);
const mat3 reflect_transform = mat3(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

void main()
{
	FragColor = vec4(0.0f);
	for (int i = 0; i < n_lights; ++i)
	{
		const vec3 t_normal = 
			normalize(
				v_normal_space * 
				(texture(normal_map, v_tex_coord).rgb * 2.0f - 1.0f) * normal_scaling
			);
		const vec3 light_direction = lights[i].position - v_position;
		const vec3 normalized_light_direction = normalize(light_direction);
		const vec3 reflected_eye_direction = normalize(reflect(v_position - eye, t_normal));
		const vec3 light_intensity = lights[i].color.xyz * lights[i].intensity / dot(light_direction, light_direction);

		mat3 color = mat3(
			texture(ambient, v_tex_coord).xyz, 
			texture(diffuse, v_tex_coord).xyz, 
			mix(texture(specular, v_tex_coord).xyz, texture(cubemap, reflect_transform * reflected_eye_direction).xyz, 0.5f).xyz
		);

		const mat3 intensity = mat3(
			1.0f  * light_intensity, 
			max(dot(normalized_light_direction, t_normal), min_diffuse) * light_intensity, 
			max(dot(normalized_light_direction, reflected_eye_direction), zero) * light_intensity * 0.0f
		);

		const mat3 k = mat3(ka, kd, ks);
		color = matrixCompMult(matrixCompMult(intensity, k), color);
		FragColor.xyz += color * vec3(1.0f, 1.0f, 1.0f);
	}

	float sh = textureProj(shadow_map, v_shadow_tex - vec4(0.0f, 0.0f, 0.1f, 0.0f));

	FragColor.xyz *= max(sh, 0.4f);

	FragColor.a = tr[0];
	FragColor = clamp(FragColor, zero, one);
}