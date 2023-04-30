#include <map>
#include <fstream>
#include <vector>
#include <string>
#include <string_view>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include "opengl/shader.hpp"
#include "utilities/definition.hpp"

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
	source_path(shader_source_path)
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
		if (id)
			glDeleteShader(id);
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

ShaderSource::ShaderSource(ShaderSource&& other) noexcept:
	id(other.id), source_path(other.source_path), type(other.type)
{
	other.id = 0;
}

ShaderSource& ShaderSource::operator=(ShaderSource&& other) noexcept
{
	if (this == &other)
		return *this;
	id = other.id;
	other.id = 0;
	source_path = other.source_path;
	type = other.type;
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

const std::filesystem::path& ShaderSource::get_path() const
{
	return source_path;
}

void Shader::check(const std::vector<ShaderSource>& sources)
{
	constexpr int LOG_SIZE = 1024;
	int success;
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		char log[LOG_SIZE];
		glGetProgramInfoLog(id, LOG_SIZE, nullptr, log);
		if (id)
			glDeleteProgram(id);
		std::stringstream ss;
		ss << "[Error] Failed to link shader: \n";
		for (const ShaderSource& source: sources)
		{
			ss << source.get_path() << std::endl;
		}
		throw std::runtime_error(ss.str());
	}
}

Shader::~Shader()
{
	if (id) 
		glDeleteProgram(id);
}

Shader::Shader(Shader&& other) noexcept: id(other.id)
{
	other.id = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
	if (this == &other) return *this;
	id = other.id;
	other.id = 0;
	return *this;
}

Shader::operator GLuint()
{
	return id;
}

Shader::operator GLuint() const
{
	return id;
}

void Shader::use() const
{
	glUseProgram(id);
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

void Shader::set(const std::string_view& variable_name, const glm::vec2& value, int count) const noexcept
{
   glUniform2fv(glGetUniformLocation(id, variable_name.data()), count, glm::value_ptr(value));
}

void Shader::set(const std::string_view& variable_name, const glm::vec3& value, int count) const noexcept
{
   glUniform3fv(glGetUniformLocation(id, variable_name.data()), count, glm::value_ptr(value));
}

void Shader::set(const std::string_view& variable_name, const glm::vec4& value, int count) const noexcept
{
   glUniform4fv(glGetUniformLocation(id, variable_name.data()), count, glm::value_ptr(value));
}

void Shader::set(const std::string_view& variable_name, const glm::mat2& value, int count) const noexcept
{
   glUniformMatrix2fv(glGetUniformLocation(id, variable_name.data()), count, GL_FALSE, glm::value_ptr(value));
}

void Shader::set(const std::string_view& variable_name, const glm::mat3& value, int count) const noexcept
{
   glUniformMatrix3fv(glGetUniformLocation(id, variable_name.data()), count, GL_FALSE, glm::value_ptr(value));
}

void Shader::set(const std::string_view& variable_name, const glm::mat4& value, int count) const noexcept
{
   glUniformMatrix4fv(glGetUniformLocation(id, variable_name.data()), count, GL_FALSE, glm::value_ptr(value));
}

void Shader::set(GLuint location, unsigned int value) const noexcept
{
   glUniform1ui(location, value);
}

void Shader::set(GLuint location, int value) const noexcept
{
   glUniform1i(location, value);
}

void Shader::set(GLuint location, float value) const noexcept
{
   glUniform1f(location, value);
}

void Shader::set(GLuint location, const glm::vec2& value, int count) const noexcept
{
   glUniform2fv(location, count, glm::value_ptr(value));
}

void Shader::set(GLuint location, const glm::vec3& value, int count) const noexcept
{
   glUniform3fv(location, count, glm::value_ptr(value));
}

void Shader::set(GLuint location, const glm::vec4& value, int count) const noexcept
{
   glUniform4fv(location, count, glm::value_ptr(value));
}

void Shader::set(GLuint location, const glm::mat2& value, int count) const noexcept
{
   glUniformMatrix2fv(location, count, GL_FALSE, glm::value_ptr(value));
}

void Shader::set(GLuint location, const glm::mat3& value, int count) const noexcept
{
   glUniformMatrix3fv(location, count, GL_FALSE, glm::value_ptr(value));
}

void Shader::set(GLuint location, const glm::mat4& value, int count) const noexcept
{
   glUniformMatrix4fv(location, count, GL_FALSE, glm::value_ptr(value));

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