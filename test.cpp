#define GLM_ENABLE_EXPERIMENTAL
#include "include/glm/ext.hpp"
#include "include/glm/glm.hpp"
#include <iostream>
#include <vector>
#include <array>
int main()
{
	std::array<int, 1> v = {};
	std::cout << sizeof(v) << std::endl;
}