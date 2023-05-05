#pragma once
#include <map>
#include "opengl/shader.hpp"
#include "utilities/definition.hpp"

class ShaderContainer
{
public:
	enum DEFAULT_SHADER_FLAG
	{
		NORMAL = 1,
		NORMAL_MAP = 3,
		RIGGING = 4,
		PONG = 8,
		COLOR = 16,
	};

	template <typename...Arg>
	Shader* add_shader(const std::string& name, Arg...arg)
	{
		auto it = container.find(name);
		if (it != container.end())
			return &it->second;
		container.emplace(name, Shader(arg...));
		return &container[name];
	}

	Shader* get_shader(const std::string& name)
	{
		auto it = container.find(name);
		if (it == container.end())
			return nullptr;
		return &it->second;
	}

	bool delete_shader(const std::string& name)
	{
		auto it = container.find(name);
		if (it == container.end())
			return false;
		container.erase(it);
		return true;
	}

	static Shader* default_shader(unsigned int flag)
	{
		static Shader rigging_normalmap(root_path / "./shader/normal_rigging.vert", root_path / "./shader/normal.frag");
		static Shader normalmap(root_path / "./shader/normal.vert", root_path / "./shader/normal.frag");
		switch (flag)
		{
			case (RIGGING | NORMAL_MAP):
				return &rigging_normalmap;
			case (RIGGING | NORMAL):
				return &rigging_normalmap;
			case (NORMAL_MAP):
				return &normalmap;
			case (NORMAL):
				return &normalmap;
			default:
				throw std::runtime_error("[ERROR] Default shader not found: " + std::to_string(flag));
		}
	}

	static Shader* default_shadow_shader(unsigned int flag)
	{
		static Shader rigging_normalmap(root_path / "./shader/shadow_normal_rigging.vert", root_path / "./shader/shadow_normal.frag");
		static Shader normalmap(root_path / "./shader/shadow_normal.vert", root_path / "./shader/shadow_normal.frag");
		switch (flag)
		{
			case (RIGGING | NORMAL_MAP):
				return &rigging_normalmap;
			case (RIGGING | NORMAL):
				return &rigging_normalmap;
			case (NORMAL_MAP):
				return &normalmap;
			case (NORMAL):
				return &normalmap;
			default:
				throw std::runtime_error("[ERROR] Default shadow shader not found: " + std::to_string(flag));
		}
	}

	static Shader* default_shadow_map_shader(unsigned int flag)
	{
		static Shader rigging(root_path / "./shader/shadow_map_rigging.vert", root_path / "./shader/shadow_map.frag");
		static Shader none(root_path / "./shader/shadow_map_basic.vert", root_path / "./shader/shadow_map.frag");
		flag &= (~NORMAL);
		flag &= (~NORMAL_MAP);
		switch (flag)
		{
			case (RIGGING):
				return &rigging;
			case (0):
				return &none;
			default:
				throw std::runtime_error("[ERROR] Default shadow map shader not found: " + std::to_string(flag));
		}
	}

	static Shader* default_shadow_env_shader(unsigned int flag)
	{
		static Shader rigging_normalmap(root_path / "./shader/shadow_normal_rigging.vert", root_path / "./shader/shadow_normal_env.frag");
		static Shader normalmap(root_path / "./shader/shadow_normal.vert", root_path / "./shader/shadow_normal_env.frag");
		switch (flag)
		{
			case (RIGGING | NORMAL_MAP):
				return &rigging_normalmap;
			case (RIGGING | NORMAL):
				return &rigging_normalmap;
			case (NORMAL_MAP):
				return &normalmap;
			case (NORMAL):
				return &normalmap;
			default:
				throw std::runtime_error("[ERROR] Default shadow env shader not found: " + std::to_string(flag));
		}
	}

private:
	std::map<std::string, Shader> container;
};