#version 460 core

layout (location = 1, binding = 1) uniform sampler2D ambient;
layout (location = 2, binding = 2) uniform sampler2D diffuse;
layout (location = 3, binding = 3) uniform sampler2D specular;
layout (location = 4, binding = 4) uniform sampler2D normal_map;
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

void main()
{
	FragColor = vec4(0.0f);
	for (int i = 0; i < n_lights; ++i)
	{
		const vec3 t_normal = 
			normalize(
				v_normal_space * 
				(texture(normal_map, v_tex_coord).rgb * 2.0f - 1.0f)
			);
		const vec3 light_direction = lights[i].position - v_position;
		const float light_distance_sqare = dot(light_direction, light_direction);
		const vec3 normalized_light_direction = normalize(light_direction);
		const vec3 eye_from_position = v_position - eye;
		const vec3 light_intensity = lights[i].color.xyz * lights[i].intensity / light_distance_sqare;

		vec4 ambient_color = texture(ambient, v_tex_coord);
		vec3 ambient_intensity = light_intensity;
		ambient_color.rgb *= ambient_intensity;
		ambient_color.rgb *= ka;

		vec4 diffuse_color = texture(diffuse, v_tex_coord);
		vec3 diffuse_intensity = light_intensity * max(dot(normalized_light_direction, t_normal), zero);
		diffuse_color.rgb *= diffuse_intensity;
		diffuse_color.rgb *= kd;


		vec4 specular_color = texture(specular, v_tex_coord);
		vec3 specular_intensity = light_intensity *	max(dot(normalize(eye_from_position), reflect(normalized_light_direction, t_normal)), zero);
		specular_color.rgb *= specular_intensity;
		specular_color.rgb *= ks;

		FragColor += ambient_color + diffuse_color + specular_color;
	}

	float sh = textureProj(shadow_map, v_shadow_tex - vec4(0.0f, 0.0f, 0.1f, 0.0f));

	FragColor *= max(sh, 0.4f);

	FragColor.a = one;
	FragColor = clamp(FragColor, zero, one);
}