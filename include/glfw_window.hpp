#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <stdexcept>
#include <string>
#include <string_view>

class Window
{
public:
	
	struct Builder
	{
		glm::ivec2 window_size = {1280, 720};
		bool full_screen = false;
		std::string window_name = "noname";
		int samples = 4;
		int major_viersion = 3;
		int minor_viersion = 3;
		unsigned int profile = GLFW_OPENGL_CORE_PROFILE;

		[[nodiscard]] Window create() const;
	};
	
	// template <typename F, typename... ARG>
	// void loop(F f, ARG&...arg)
	// {
	// 	glEnable(GL_DEPTH_TEST);
	// 	while (!glfwWindowShouldClose(window))
	// 	{
	// 		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 		f(arg...);
	// 		glfwSwapBuffers(window);
	// 		glfwPollEvents();
	// 	}
	// }

	// template <typename F, typename... ARG>
	// void loop(F&& f, ARG&...arg)
	// {
	// 	glEnable(GL_DEPTH_TEST);
	// 	while (!glfwWindowShouldClose(window))
	// 	{
	// 		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 		f(arg...);
	// 		glfwSwapBuffers(window);
	// 		glfwPollEvents();
	// 	}
	// }

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&& other);
	Window& operator=(Window&& other);
	~Window();

	operator GLFWwindow*(){ return window; }
	
	void set_viewport()
	{
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		glViewport(0, 0, w, h);
	}

	glm::ivec2 get_window_size() const
	{
		glm::ivec2 result;
		glfwGetWindowSize(window, &result.x, &result.y);
		return result;
	}

private:
	GLFWwindow* window;

	Window(const Builder& builder)
	{
		glfwWindowHint(GLFW_SAMPLES, builder.samples);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, builder.major_viersion);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, builder.minor_viersion);
		glfwWindowHint(GLFW_OPENGL_PROFILE, builder.profile);
		#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE;
		#endif

		GLFWmonitor* fullscreen_monitor = nullptr;
		if (builder.full_screen)
		{
			int count = 0;
			GLFWmonitor** monitors = glfwGetMonitors(&count);
		}

		window = glfwCreateWindow(
			builder.window_size.x, 
			builder.window_size.y,
			builder.window_name.data(),
			fullscreen_monitor,
			nullptr
		);
		if (!window)
		{
			throw std::runtime_error("Failed to create window");
		}
		glfwMakeContextCurrent(window);
		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
		{
			throw std::runtime_error("Failed to initialize GLAD");
		}
	}

	struct Glfw
	{
		Glfw();
		~Glfw();
	};
	inline static Glfw glfw_init = Glfw();
};