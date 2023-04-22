#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

inline
std::ostream& operator<<(std::ostream& os, const glm::vec2& v)
{
	return os << glm::to_string(v);
}

inline
std::ostream& operator<<(std::ostream& os, const glm::vec3& v)
{
	return os << glm::to_string(v);
}

inline
std::ostream& operator<<(std::ostream& os, const glm::vec4& v)
{
	return os << glm::to_string(v);
}

inline
std::ostream& operator<<(std::ostream& os, const glm::mat2& v)
{
	return os << glm::to_string(v);
}

inline
std::ostream& operator<<(std::ostream& os, const glm::mat3& v)
{
	return os << glm::to_string(v);
}

inline
std::ostream& operator<<(std::ostream& os, const glm::mat4& v)
{
	return os << glm::to_string(v);
}