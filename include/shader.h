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
	ShaderSource(ShaderSource&& other);
	ShaderSource& operator=(ShaderSource&& other);

	~ShaderSource();

	operator GLuint();
	operator GLuint() const;
	GLuint get_id() const;

private:
	GLuint id = 0;
	std::filesystem::path source_path;
	Type type = Type::NONE;
	inline static const std::filesystem::path root_path = ROOT;

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
	
	void set(const std::string_view& variable_name, unsigned int value) const noexcept;
	void set(const std::string_view& variable_name, int value) const noexcept;
	void set(const std::string_view& variable_name, float value) const noexcept;
	void set(const std::string_view& variable_name, glm::vec2 value) const noexcept;
	void set(const std::string_view& variable_name, glm::vec3 value) const noexcept;
	void set(const std::string_view& variable_name, glm::vec4 value) const noexcept;
	void set(const std::string_view& variable_name, glm::mat2 value) const noexcept;
	void set(const std::string_view& variable_name, glm::mat3 value) const noexcept;
	void set(const std::string_view& variable_name, glm::mat4 value) const noexcept;
	void set(const std::string_view& variable_name, int count, glm::vec2 value) const noexcept;
	void set(const std::string_view& variable_name, int count, glm::vec3 value) const noexcept;
	void set(const std::string_view& variable_name, int count, glm::vec4 value) const noexcept;
	void set(const std::string_view& variable_name, int count, glm::mat2 value) const noexcept;
	void set(const std::string_view& variable_name, int count, glm::mat3 value) const noexcept;
	void set(const std::string_view& variable_name, int count, glm::mat4 value) const noexcept;
	void set(int unit, int value) noexcept;
	void set(int unit, float value) noexcept;
	void set(int unit, glm::vec2 value) noexcept;
	void set(int unit, glm::vec3 value) noexcept;
	void set(int unit, glm::vec4 value) noexcept;
	void set(int unit, glm::mat2 value) noexcept;
	void set(int unit, glm::mat3 value) noexcept;
	void set(int unit, glm::mat4 value) noexcept;
	void set(int unit, int count, glm::vec2 value) noexcept;
	void set(int unit, int count, glm::vec3 value) noexcept;
	void set(int unit, int count, glm::vec4 value) noexcept;
	void set(int unit, int count, glm::mat2 value) noexcept;
	void set(int unit, int count, glm::mat3 value) noexcept;
	void set(int unit, int count, glm::mat4 value) noexcept;

private:
	inline static const std::filesystem::path root_path = ROOT;
	GLuint id = 0;

	void check();
};
