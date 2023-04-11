#include <map>
#include <fstream>
#include <vector>
#include <string>
#include <string_view>
#include "shader.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

static ShaderSource::Type get_shader_type(const std::string& extension)
{
	static std::map<std::string, ShaderSource::Type> types = {
		{".vert", ShaderSource::Type::VERTEX},
		{".frag", ShaderSource::Type::FRAGMENT},
		{".comp", ShaderSource::Type::COMPUTE},
	};
	auto it = types.find(extension);
	if (it != types.end())
		return it->second;
	else
		return ShaderSource::Type::NONE;
}

ShaderSource::ShaderSource(const std::filesystem::path& shader_source_path):
	source_path(root_path / shader_source_path)
{
	std::string extension = source_path.extension().string();
	type = get_shader_type(extension);
	
	if (type == Type::NONE)
		throw std::runtime_error("[Error] Wrong extension: " + source_path.string());
	id = glCreateShader(static_cast<int>(type));

	std::ifstream source_file_stream(source_path);
	if (!source_file_stream.is_open())
	{
		throw std::runtime_error("[Error] Failed to open shader source file: "s + 
			source_path.string());
	}
	std::stringstream ss;
	ss << source_file_stream.rdbuf();
	std::string source = ss.str();
	const char* source_ptr = source.data();
	source_file_stream.read(source.data(), source.size());
	glShaderSource(id, 1, &source_ptr, nullptr);
	glCompileShader(id);
	check();
}

void ShaderSource::check()
{
	constexpr int LOG_SIZE = 1024;
	int success;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char log[LOG_SIZE];
		glGetShaderInfoLog(id, LOG_SIZE, nullptr, log);
		std::stringstream ss;
		ss << "[Error] Failed to Compile shader: "
			<< source_path.string() << '\n'
			<< log << std::endl;
		throw std::runtime_error(ss.str());
	}
}

ShaderSource::~ShaderSource()
{
	if (id)
		glDeleteShader(id);
}

ShaderSource::ShaderSource(ShaderSource&& other):
	id(other.id)
{
	other.id = 0;
}

ShaderSource& ShaderSource::operator=(ShaderSource&& other)
{
	id = other.id;
	other.id = 0;
	return *this;
}

ShaderSource::operator GLuint()
{
	return id;
}

ShaderSource::operator GLuint() const
{
	return id;
}

GLuint ShaderSource::get_id() const
{
	return id;
}

void Shader::check()
{
	constexpr int LOG_SIZE = 1024;
	int success;
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		char log[LOG_SIZE];
		glGetProgramInfoLog(id, LOG_SIZE, nullptr, log);
		throw std::runtime_error("[Error] Failed to link shader");
	}
}

Shader::Shader(std::initializer_list<std::filesystem::path> path)
{
	id = glCreateProgram();
	std::vector<ShaderSource> sources;
	sources.reserve(path.size());
	for (const auto& p: path)
	{
		sources.emplace_back(root_path / p);
		glAttachShader(id, sources.back().get_id());
	}
	glLinkProgram(id);
	check();
}

Shader::Shader(std::initializer_list<ShaderSource> shaders)
{
	id = glCreateProgram();
	for (const auto& shader: shaders)
	{
		glAttachShader(id, shader);
	}
	glLinkProgram(id);
	check();
}

Shader::~Shader()
{
	if (id) 
		glDeleteProgram(id);
}

Shader::operator GLuint()
{
	return id;
}

Shader::operator GLuint() const
{
	return id;
}

void Shader::set(const std::string_view& variable_name, unsigned int value) const noexcept
{
   glUniform1ui(glGetUniformLocation(id, variable_name.data()), value);
}

void Shader::set(const std::string_view& variable_name, int value) const noexcept
{
   glUniform1i(glGetUniformLocation(id, variable_name.data()), value);
}

void Shader::set(const std::string_view& variable_name, float value) const noexcept
{
   glUniform1f(glGetUniformLocation(id, variable_name.data()), value);
}

void Shader::set(const std::string_view& variable_name, glm::vec2 value) const noexcept
{
   glUniform2fv(glGetUniformLocation(id, variable_name.data()), 1, glm::value_ptr(value));
}

void Shader::set(const std::string_view& variable_name, glm::vec3 value) const noexcept
{
   glUniform3fv(glGetUniformLocation(id, variable_name.data()), 1, glm::value_ptr(value));
}

void Shader::set(const std::string_view& variable_name, glm::vec4 value) const noexcept
{
   glUniform4fv(glGetUniformLocation(id, variable_name.data()), 1, glm::value_ptr(value));
}

void Shader::set(const std::string_view& variable_name, glm::mat2 value) const noexcept
{
   glUniformMatrix2fv(glGetUniformLocation(id, variable_name.data()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set(const std::string_view& variable_name, glm::mat3 value) const noexcept
{
   glUniformMatrix3fv(glGetUniformLocation(id, variable_name.data()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set(const std::string_view& variable_name, glm::mat4 value) const noexcept
{
   glUniformMatrix4fv(glGetUniformLocation(id, variable_name.data()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set(const std::string_view& variable_name, int count, glm::vec2 value) const noexcept
{
   glUniform2fv(glGetUniformLocation(id, variable_name.data()), count, glm::value_ptr(value));
}

void Shader::set(const std::string_view& variable_name, int count, glm::vec3 value) const noexcept
{
   glUniform3fv(glGetUniformLocation(id, variable_name.data()), count, glm::value_ptr(value));
}

void Shader::set(const std::string_view& variable_name, int count, glm::vec4 value) const noexcept
{
   glUniform4fv(glGetUniformLocation(id, variable_name.data()), count, glm::value_ptr(value));
}

void Shader::set(const std::string_view& variable_name, int count, glm::mat2 value) const noexcept
{
   glUniformMatrix2fv(glGetUniformLocation(id, variable_name.data()), count, GL_FALSE, glm::value_ptr(value));
}

void Shader::set(const std::string_view& variable_name, int count, glm::mat3 value) const noexcept
{
   glUniformMatrix3fv(glGetUniformLocation(id, variable_name.data()), count, GL_FALSE, glm::value_ptr(value));
}

void Shader::set(const std::string_view& variable_name, int count, glm::mat4 value) const noexcept
{
   glUniformMatrix4fv(glGetUniformLocation(id, variable_name.data()), count, GL_FALSE, glm::value_ptr(value));
}

void Shader::set(int unit, int value) noexcept
{
   glUniform1i(unit, value);
}

void Shader::set(int unit, float value) noexcept
{
   glUniform1f(unit, value);
}

void Shader::set(int unit, glm::vec2 value) noexcept
{
   glUniform2fv(unit, 1, glm::value_ptr(value));
}

void Shader::set(int unit, glm::vec3 value) noexcept
{
   glUniform3fv(unit, 1, glm::value_ptr(value));
}

void Shader::set(int unit, glm::vec4 value) noexcept
{
   glUniform4fv(unit, 1, glm::value_ptr(value));
}

void Shader::set(int unit, glm::mat2 value) noexcept
{
   glUniformMatrix2fv(unit, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set(int unit, glm::mat3 value) noexcept
{
   glUniformMatrix3fv(unit, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set(int unit, glm::mat4 value) noexcept
{
   glUniformMatrix4fv(unit, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set(int unit, int count, glm::vec2 value) noexcept
{
   glUniform2fv(unit, count, glm::value_ptr(value));
}

void Shader::set(int unit, int count, glm::vec3 value) noexcept
{
   glUniform3fv(unit, count, glm::value_ptr(value));
}

void Shader::set(int unit, int count, glm::vec4 value) noexcept
{
   glUniform4fv(unit, count, glm::value_ptr(value));
}

void Shader::set(int unit, int count, glm::mat2 value) noexcept
{
   glUniformMatrix2fv(unit, count, GL_FALSE, glm::value_ptr(value));
}

void Shader::set(int unit, int count, glm::mat3 value) noexcept
{
   glUniformMatrix3fv(unit, count, GL_FALSE, glm::value_ptr(value));
}

void Shader::set(int unit, int count, glm::mat4 value) noexcept
{
   glUniformMatrix4fv(unit, count, GL_FALSE, glm::value_ptr(value));

}

void dispatch_compute(int x, int y, int z)
{
   x >>= 5;
   x += 1;
   y >>= 5;
   y += 1;
   z >>= 5;
   z += 1;
   glDispatchCompute(x, y, z);
}