#pragma once
#include <glad/glad.h>
#include <filesystem>
#include <initializer_list>
#include <glm/glm.hpp>
#include <iostream>
#include "vertex_attrib.hpp"

class ShaderSource
{
public:
	enum class Type
	{
		VERTEX = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER,
		COMPUTE = GL_COMPUTE_SHADER,
		NONE
	};

	ShaderSource(const std::filesystem::path& source_path);
	ShaderSource(const ShaderSource&) = delete;
	ShaderSource& operator=(const ShaderSource&) = delete;
	ShaderSource(ShaderSource&& other) noexcept;
	ShaderSource& operator=(ShaderSource&& other) noexcept;

	~ShaderSource();

	operator GLuint();
	operator GLuint() const;
	GLuint get_id() const;
	const std::filesystem::path& get_path() const;

private:
	GLuint id = 0;
	std::filesystem::path source_path;
	Type type = Type::NONE;

	void check();
};

class Shader
{
public:
	enum class VERTEX_INPUT_LOCATION
	{
		// vertex attribs
		POSITION = static_cast<int>(VERTEX_ATTRIB::POSITION),
		NORMAL = static_cast<int>(VERTEX_ATTRIB::NORMAL),
		TANGENT = static_cast<int>(VERTEX_ATTRIB::TANGENT),
		BI_TANGENT = static_cast<int>(VERTEX_ATTRIB::BI_TANGENT),
		TEX_COORD = static_cast<int>(VERTEX_ATTRIB::TEX_COORD),
		COLOR = static_cast<int>(VERTEX_ATTRIB::COLOR),
		WEIGHTS = static_cast<int>(VERTEX_ATTRIB::WEIGHTS),
		BONE_INDICES = static_cast<int>(VERTEX_ATTRIB::BONE_INDICES),
	};

	enum class TEXTURE_LOCATION_BINDING
	{
		AMBIENT = 1,
		DIFFUSE = 2,
		SPECULAR = 3,
		NORMAL_MAP = 4,
		ENV_MAP = 9,
		SHADOW_MAP = 10,
	};

	enum class UNIFORM_LOCATION
	{
		// model
		MODEL = 11,

		// camera
		VIEW = 21,
		PROJECTION = 22,

		// shadow
		SHADOW_VIEW = 31,
		SHADOW_PROJECTION = 32,

		// texture
		KA = 41,
		KD = 42,
		KS = 43,
		TR = 44,
		
		// light
		N_LIGHT = 51,
		EYE = 52,
	};

	enum class UNIFORM_BLOCK_BINDING
	{
		TRANSFORM = 1,
		LIGHT = 2,
	};

	template <typename...Arg>
	Shader(Arg...arg): id(0)
	{
		std::vector<ShaderSource> sources;
		sources.reserve(6);
		load_source(sources, arg...);
		id = glCreateProgram();
		for (ShaderSource& source: sources)
		{
			glAttachShader(id, source);
		}
		glLinkProgram(id);
		check(sources);
	}

	template <typename PathType, typename...Arg>
	void load_source(std::vector<ShaderSource>& sources, const PathType& path, const Arg&...arg)
	{
		sources.emplace_back(path);
		load_source(sources, arg...);
	}

	void load_source(std::vector<ShaderSource>& sources)
	{
		return;
	}

	Shader(Shader&& other) noexcept;
	Shader& operator=(Shader&& other) noexcept;
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	~Shader();

	operator GLuint();
	operator GLuint() const;
	
	void use() const;
	void set(const std::string_view& variable_name, unsigned int value) const noexcept;
	void set(const std::string_view& variable_name, int value) const noexcept;
	void set(const std::string_view& variable_name, float value) const noexcept;
	void set(const std::string_view& variable_name, const glm::vec2& value, int count = 1) const noexcept;
	void set(const std::string_view& variable_name, const glm::vec3& value, int count = 1) const noexcept;
	void set(const std::string_view& variable_name, const glm::vec4& value, int count = 1) const noexcept;
	void set(const std::string_view& variable_name, const glm::mat2& value, int count = 1) const noexcept;
	void set(const std::string_view& variable_name, const glm::mat3& value, int count = 1) const noexcept;
	void set(const std::string_view& variable_name, const glm::mat4& value, int count = 1) const noexcept;
	void set(GLuint location, GLuint value) const noexcept;
	void set(GLuint location, int value) const noexcept;
	void set(GLuint location, float value) const noexcept;
	void set(GLuint location, const glm::vec2& value, int count = 1) const noexcept;
	void set(GLuint location, const glm::vec3& value, int count = 1) const noexcept;
	void set(GLuint location, const glm::vec4& value, int count = 1) const noexcept;
	void set(GLuint location, const glm::mat2& value, int count = 1) const noexcept;
	void set(GLuint location, const glm::mat3& value, int count = 1) const noexcept;
	void set(GLuint location, const glm::mat4& value, int count = 1) const noexcept;

private:
	GLuint id = 0;
	
	void check(const std::vector<ShaderSource>& sources);
};
