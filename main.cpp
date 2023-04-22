#include <iostream>
#include "glfw_window.h"
#include "opengl.h"
#include "camera.h"

#include "model_load.h"

#include "glm_to_string.h"

int main()
{
	try
	{
		using namespace std::string_literals;
		using namespace std::chrono_literals;

		// Window window = Window::Builder{.full_screen = false}.create();
		// std::vector<Vertex> vertices = {
		// 	{{-0.5f, -0.5f,  0.0f}, {0.2f, 0.4f, 0.3f}, {0.0f, 0.0f}},
		// 	{{ 0.5f, -0.5f,  0.0f}, {0.1f, 0.2f, 0.4f}, {1.0f, 0.0f}},
		// 	{{ 0.5f,  0.5f,  0.0f}, {0.8f, 0.4f, 0.3f}, {1.0f, 1.0f}},
		// 	{{-0.5f,  0.5f,  0.0f}, {0.8f, 0.6f, 0.2f}, {0.0f, 1.0f}},
		// };
		// std::vector<GLuint> indices = {
		// 	0, 1, 2,
		// 	0, 2, 3
		// };
		// VertexArray<Vertex> vertex_array;
		// Buffer<Vertex> vertex_buffer(vertices);
		// ElementBuffer element_buffer(indices);
		// vertex_array.bind(vertex_buffer, element_buffer);

		// Shader shader{"./shader/basic.vert", "./shader/basic.frag"};

		// Texture texture("./lenna.png");
		// texture.bind(shader, "tex", 1);
		// window.loop(
		// 	[&]()
		// 	{
		// 		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) 
		// 			glfwSetWindowShouldClose(window, true);
		// 		shader.use();
		// 		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		// 		vertex_array.draw(shader);
		// 	}
		// );

		Window window = Window::Builder{.full_screen = false}.create();
		Shader shader{"./shader/basic3d.vert", "./shader/basic3d.frag"};
		auto model = AssimpLoader::Options{.flip_uv = false, .limit_bone_weights = true}.load(
			// root_path/"resource/backpack/backpack.obj"
			// root_path/"resource/walking_man/rp_nathan_animated_003_walking.fbx"
			root_path/"resource/dragon/Dragon_Baked_Actions_fbx_7.4_binary.fbx"
		);

		auto bounding_box = model->getBoundingBox();
		std::cout 
			<< glm::to_string(bounding_box.first) << ", " 
			<< glm::to_string(bounding_box.second) << std::endl;

		glm::vec3 cam_dir = bounding_box.first - bounding_box.second;
		glm::vec3 cam_pos = bounding_box.second - cam_dir;

		float size = glm::length(bounding_box.first - bounding_box.second);

		Camera camera(
			cam_pos,
			cam_dir,
			{0, 0, 1},
			{1280, 720},
			size * 0.01f,
			size * 100.0f
		);
		Camera origin = camera;
		std::cout << camera;

		Microseconds time(0);
		std::chrono::time_point tp = std::chrono::high_resolution_clock::now();
		window.loop(
			[&]()
			{
				if (glfwGetKey(window, GLFW_KEY_ESCAPE))	glfwSetWindowShouldClose(window, true);
				if (glfwGetKey(window, GLFW_KEY_W))			camera.move_forward(size * 0.05f);
				if (glfwGetKey(window, GLFW_KEY_S))			camera.move_backward(size * 0.05f);
				if (glfwGetKey(window, GLFW_KEY_A))			camera.move_left(size * 0.05f);
				if (glfwGetKey(window, GLFW_KEY_D))			camera.move_right(size * 0.05f);
				if (glfwGetKey(window, GLFW_KEY_SPACE))		camera.move_up(size * 0.05f);
				if (glfwGetKey(window, GLFW_KEY_X))			camera.move_down(size * 0.05f);
				if (glfwGetKey(window, GLFW_KEY_UP))		camera.pitch_up(0.05f);
				if (glfwGetKey(window, GLFW_KEY_DOWN))		camera.pitch_down(0.05f);
				if (glfwGetKey(window, GLFW_KEY_LEFT))		camera.turn_left(0.05f);
				if (glfwGetKey(window, GLFW_KEY_RIGHT))		camera.turn_right(0.05f);
				
				if (glfwGetKey(window, GLFW_KEY_Z))	camera = origin;
				if (glfwGetKey(window, GLFW_KEY_C))	{ time = 0us; tp = std::chrono::high_resolution_clock::now(); }
				if (glfwGetKey(window, GLFW_KEY_V))	{ time -= 20ms; }

				shader.use();
				glClearColor(0.1f, 0.1f, 0.1f, 1.0f);


				camera.update_view();
				model->draw(shader, 0, time, camera.view, camera.projection, 
					{
						1.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 1.0f, 0.0f,
						0.0f, 1.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 1.0f,
					}
				);
				time += 20ms;
				tp += 10ms;
				std::this_thread::sleep_until(tp);
			}
		);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}