#include "glfw_window.hpp"

Window::Glfw::Glfw()
{
	if (!glfwInit())
		throw std::runtime_error("[Error] glfwInit() failed");
}

Window::Glfw::~Glfw()
{
	glfwTerminate();
}

Window Window::Builder::create() const
{
	return Window(*this);
}

Window::Window(Window&& other):
	window(other.window)
{
	other.window = nullptr;
}

Window& Window::operator=(Window&& other)
{
	window = other.window;
	other.window = nullptr;
	return *this;
}

Window::~Window()
{
	glfwDestroyWindow(window);
}