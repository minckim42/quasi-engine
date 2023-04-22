#pragma once
#include <glad/glad.h>
#include <filesystem>
#include <initializer_list>
#include <glm/glm.hpp>

#include <iostream>
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

private:
	GLuint id = 0;
	std::filesystem::path source_path;
	Type type = Type::NONE;

	void check();
};

class Shader
{
public:
	Shader(std::initializer_list<std::filesystem::path> path);
	Shader(std::initializer_list<ShaderSource> shaders);
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

	void check();
};
