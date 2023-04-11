#include <iostream>
#include "glfw_window.h"
#include "opengl.h"
#include "vertex.h"

int main()
{
	try
	{
		using namespace std::string_literals;

		std::vector<Vertex> vertices = {
			{{-0.5f, -0.5f,  0.0f}, {0.2f, 0.4f, 0.3f}, {0.0f, 0.0f}},
			{{ 0.5f, -0.5f,  0.0f}, {0.1f, 0.2f, 0.4f}, {1.0f, 0.0f}},
			{{ 0.5f,  0.5f,  0.0f}, {0.8f, 0.4f, 0.3f}, {1.0f, 1.0f}},
			{{-0.5f,  0.5f,  0.0f}, {0.8f, 0.6f, 0.2f}, {0.0f, 1.0f}},
		};

		std::vector<GLuint> indices = {
			0, 1, 2,
			0, 2, 3
		};

		Window window = Window::Builder{.full_screen = false}.create();

		VertexArray<Vertex> vertex_array;
		Buffer<Vertex> vertex_buffer(vertices);
		ElementBuffer element_buffer(indices);
		vertex_array.bind(vertex_buffer, element_buffer);

		Shader shader{"./shader/basic.vert", "./shader/basic.frag"};

		Texture texture("./lenna.png");
		texture.bind(shader, "tex", 1);

		window.loop(
			[&]()
			{
				if (glfwGetKey(window, GLFW_KEY_ESCAPE)) 
					glfwSetWindowShouldClose(window, true);
				glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
				glUseProgram(shader);
				glBindVertexArray(vertex_array);
				vertex_array.draw(shader);
			}
		);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}