#include <iostream>
#include "glfw_window.hpp"
#include "opengl/opengl.hpp"
#include "camera.hpp"

#include "model_load.hpp"
#include "light.hpp"
#include "utilities/clock.hpp"
#include "renderer.hpp"
#include "shader_container.hpp"


#include "utilities/glm_to_string.hpp"

Mesh create_sphere(float radius, int w_div, int h_div)
{
	Mesh mesh;
	glm::vec3 top_point_base(0.0f, 0.0f, radius);
	glm::vec3 bottom_point_base(0.0f, 0.0f, -radius);
	float dh = glm::pi<float>() / h_div;
	float dw = 2.0f * glm::pi<float>() / w_div;
	for (int h = 0; h <= h_div; ++h)
	{
		glm::vec3 base = mesh.position.emplace_back(rotate(top_point_base, UNIT_Y, dh * h));
		for (int w = 1; w <= w_div; ++w)
		{
			mesh.position.emplace_back(rotate(base, UNIT_Z, dw * w));
		}
	}
	for (int h = 0; h < h_div; ++h)
	{
		for (int w = 0; w < w_div; ++w)
		{
			mesh.indices.emplace_back((h    ) * (w_div + 1) + (w    ));
			mesh.indices.emplace_back((h + 1) * (w_div + 1) + (w    ));
			mesh.indices.emplace_back((h    ) * (w_div + 1) + (w + 1));
			mesh.indices.emplace_back((h    ) * (w_div + 1) + (w + 1));
			mesh.indices.emplace_back((h + 1) * (w_div + 1) + (w    ));
			mesh.indices.emplace_back((h + 1) * (w_div + 1) + (w + 1));
		}
	}
	mesh.normal = mesh.position;
	mesh.enable_attrib();
	return mesh;
}


std::shared_ptr<Model> create_ground()
{
	std::shared_ptr<Model> result = std::make_shared<Model>();
	auto mesh = result->meshes.emplace_back(std::make_shared<Mesh>());
	mesh->position.emplace_back(-100, -100, 0);
	mesh->position.emplace_back( 100, -100, 0);
	mesh->position.emplace_back( 100,  100, 0);
	mesh->position.emplace_back(-100,  100, 0);
	mesh->indices = {0, 1, 2, 0, 2, 3};
	mesh->enable_attrib();
	return result;
}

std::shared_ptr<Model> load_building()
{
	std::shared_ptr<Model> model = std::make_shared<Model>();
	model->children.emplace_back(AssimpLoader().load(root_path/"resource/building/sylvan_furniture.fbx"));
	model->children.emplace_back(AssimpLoader().load(root_path/"resource/building/sylvan_ground.fbx"));
	model->children.emplace_back(AssimpLoader().load(root_path/"resource/building/sylvan_mullion.fbx"));
	model->children.emplace_back(AssimpLoader().load(root_path/"resource/building/sylvan_rail.fbx"));
	model->children.emplace_back(AssimpLoader().load(root_path/"resource/building/sylvan_solid.fbx"));
	model->children.emplace_back(AssimpLoader().load(root_path/"resource/building/sylvan_site.fbx"));
	model->children.emplace_back(AssimpLoader().load(root_path/"resource/building/sylvan_glass.fbx"));
	model->transform = glm::scale(glm::vec3(0.1f));
	return model;
}

int main()
{
	try
	{
		using namespace std::string_literals;
		using namespace std::chrono_literals;

		Window window = Window::Builder{
			.window_size = {1280, 720},
			.full_screen = false, 
		}.create();

		RendererBase renderer;
		
		ShaderContainer shader_container;

		TimePoint t0 = Clock::now();
		
		std::shared_ptr<Model> building = load_building();

		// auto backpack = AssimpLoader(
		// 	AssimpLoader::Options{
		// 		.flip_uv = true,
		// 		.limit_bone_weights = true,
		// 		.shader_container = &shader_container
		// 	}
		// ).load(root_path/"resource/backpack/backpack.obj");
		// backpack->transform = glm::mat4 {
		// 	glm::vec4(UNIT_X * 50.0f, 0.0f),
		// 	glm::vec4(UNIT_Z * 50.0f, 0.0f),
		// 	glm::vec4(-UNIT_Y * 50.0f, 0.0f),
		// 	glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
		// } * backpack->transform;

		auto walking_man = AssimpLoader(
			AssimpLoader::Options
			{
				.limit_bone_weights = true,
				.shader_container = &shader_container
			}
		).load(root_path/"resource/walking_man/rp_nathan_animated_003_walking.fbx");
		walking_man->transform = glm::mat4 {
			glm::vec4(UNIT_X, 0.0f),
			glm::vec4(UNIT_Z, 0.0f),
			glm::vec4(-UNIT_Y, 0.0f),
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
		} * walking_man->transform;



		auto dragon = AssimpLoader(
			AssimpLoader::Options{
				.limit_bone_weights = true,
				.shader_container = &shader_container
			}
		).load(root_path/"resource/dragon/Dragon_Baked_Actions_fbx_7.4_binary.fbx");
		dragon->transform = 
			glm::mat4 {
				glm::vec4(UNIT_Y, 0.0f),
				glm::vec4(-UNIT_X, 0.0f),
				glm::vec4(-UNIT_Z, 0.0f),
				glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
			} * glm::scale(glm::vec3(0.001f));



		auto woman = AssimpLoader(
			AssimpLoader::Options{
				.limit_bone_weights = true,
				.shader_container = &shader_container
			}
		).load(root_path/"resource/woman/SYLT_Business_Wom-13_lowpoly_max_mapped.obj");
		// woman->transform = glm::scale(glm::vec3(10.0f));

		
		TimePoint t1 = Clock::now();

		std::cout << "loading time: " << (t1 - t0).count() / 1000000 << "ms" << std::endl;


		float size = 500.0f;
		glm::vec3 cam_dir = UNIT_X;
		glm::vec3 cam_pos = (-UNIT_X + UNIT_Z * 0.5f) * size;
		Camera camera(
			cam_pos,
			cam_dir,
			UNIT_Z,
			window.get_window_size(),
			size * 0.01f,
			size * 100.0f
		);
		Camera origin = camera;
		std::cout << camera;

		renderer.camera = camera;

		// renderer.render_requests.emplace_back(backpack, glm::mat4(1.0f), 0, 0us);
		renderer.render_requests.emplace_back(walking_man, glm::mat4(1.0f), 0, 0us);
		renderer.render_requests.emplace_back(dragon, glm::mat4(1.0f), 0, 0us);
		renderer.render_requests.emplace_back(woman, glm::mat4(1.0f), 0, 0us);
		renderer.render_requests.emplace_back(building, glm::mat4(1.0f), 0, 0us);


		renderer.point_lights = {
			{
				.position = {0.0f, -10000.0f, 20000.0f},
				.color = {1.0f, 1.0f, 0.98f, 1.0f},
				.intensity = 400000000.0f,
			},
			// {
			// 	.position = {10000, 20000, 10000},
			// 	.color = {1.0f, 1.0f, 1.0f, 1.0f},
			// 	.intensity = 200000000.0f,
			// },
			// {
			// 	.position = {-20000, 10000, 20000},
			// 	.color = {1.0f, 1.0f, 1.0f, 1.0f},
			// 	.intensity = 200000000.0f,
			// }
		};


		Microseconds time(0);
		std::chrono::time_point tp = std::chrono::high_resolution_clock::now();
		Microseconds delta_time = 20ms;
		std::thread t(
			[&]()
			{
				while (!glfwWindowShouldClose(window))
				{
					if (glfwGetKey(window, GLFW_KEY_ESCAPE))	glfwSetWindowShouldClose(window, true);
					if (glfwGetKey(window, GLFW_KEY_W))			renderer.camera.move_forward(size * 0.05f);
					if (glfwGetKey(window, GLFW_KEY_S))			renderer.camera.move_backward(size * 0.05f);
					if (glfwGetKey(window, GLFW_KEY_A))			renderer.camera.move_left(size * 0.05f);
					if (glfwGetKey(window, GLFW_KEY_D))			renderer.camera.move_right(size * 0.05f);
					if (glfwGetKey(window, GLFW_KEY_SPACE))		renderer.camera.move_up(size * 0.05f);
					if (glfwGetKey(window, GLFW_KEY_X))			renderer.camera.move_down(size * 0.05f);
					if (glfwGetKey(window, GLFW_KEY_UP))		renderer.camera.pitch_up(0.05f);
					if (glfwGetKey(window, GLFW_KEY_DOWN))		renderer.camera.pitch_down(0.05f);
					if (glfwGetKey(window, GLFW_KEY_LEFT))		renderer.camera.turn_left(0.05f);
					if (glfwGetKey(window, GLFW_KEY_RIGHT))		renderer.camera.turn_right(0.05f);
					
					if (glfwGetKey(window, GLFW_KEY_Z))	renderer.camera = origin;
					if (glfwGetKey(window, GLFW_KEY_C))	{ time = 0us; tp = std::chrono::high_resolution_clock::now(); }
					if (glfwGetKey(window, GLFW_KEY_V))	{ time -= delta_time; }

					window.set_viewport();

					

					for (auto& l: renderer.point_lights)
					{
						l.position = glm::rotateZ(l.position, delta_time.count() / 400 * 0.0001f);
					}

					for (auto& req : renderer.render_requests)
					{
						req.time = time;
					}

					// std::cout << "======rigging======\n";
					// std::cout << "ambient     : " << glGetUniformLocation(*ShaderContainer::default_shader(7), "ambient") << std::endl;
					// std::cout << "diffuse     : " << glGetUniformLocation(*ShaderContainer::default_shader(7), "diffuse") << std::endl;
					// std::cout << "eye         : " << glGetUniformLocation(*ShaderContainer::default_shader(7), "eye") << std::endl;
					// std::cout << "ka          : " << glGetUniformLocation(*ShaderContainer::default_shader(7), "ka") << std::endl;
					// std::cout << "kd          : " << glGetUniformLocation(*ShaderContainer::default_shader(7), "kd") << std::endl;
					// std::cout << "ks          : " << glGetUniformLocation(*ShaderContainer::default_shader(7), "ks") << std::endl;
					// std::cout << "model       : " << glGetUniformLocation(*ShaderContainer::default_shader(7), "model") << std::endl;
					// std::cout << "n_lights    : " << glGetUniformLocation(*ShaderContainer::default_shader(7), "n_lights") << std::endl;
					// std::cout << "normal_map  : " << glGetUniformLocation(*ShaderContainer::default_shader(7), "normal_map") << std::endl;
					// std::cout << "projection  : " << glGetUniformLocation(*ShaderContainer::default_shader(7), "projection") << std::endl;
					// std::cout << "specular    : " << glGetUniformLocation(*ShaderContainer::default_shader(7), "specular") << std::endl;
					// std::cout << "view        : " << glGetUniformLocation(*ShaderContainer::default_shader(7), "view") << std::endl;
					// std::cout << "LightBlock  : " << glGetUniformBlockIndex(*ShaderContainer::default_shader(7), "LightBlock") << std::endl;
					// std::cout << "TransformB  : " << glGetUniformBlockIndex(*ShaderContainer::default_shader(7), "TransformBlock") << std::endl;

					// std::cout << "======basic======\n";
					// std::cout << "ambient     : " << glGetUniformLocation(basic_shader, "ambient") << std::endl;
					// std::cout << "diffuse     : " << glGetUniformLocation(basic_shader, "diffuse") << std::endl;
					// std::cout << "eye         : " << glGetUniformLocation(basic_shader, "eye") << std::endl;
					// std::cout << "ka          : " << glGetUniformLocation(basic_shader, "ka") << std::endl;
					// std::cout << "kd          : " << glGetUniformLocation(basic_shader, "kd") << std::endl;
					// std::cout << "ks          : " << glGetUniformLocation(basic_shader, "ks") << std::endl;
					// std::cout << "model       : " << glGetUniformLocation(basic_shader, "model") << std::endl;
					// std::cout << "n_lights    : " << glGetUniformLocation(basic_shader, "n_lights") << std::endl;
					// std::cout << "normal_map  : " << glGetUniformLocation(basic_shader, "normal_map") << std::endl;
					// std::cout << "projection  : " << glGetUniformLocation(basic_shader, "projection") << std::endl;
					// std::cout << "specular    : " << glGetUniformLocation(basic_shader, "specular") << std::endl;
					// std::cout << "view        : " << glGetUniformLocation(basic_shader, "view") << std::endl;
					// std::cout << "LightBlock  : " << glGetUniformBlockIndex(basic_shader, "LightBlock") << std::endl;
					// std::cout << "TransformB  : " << glGetUniformBlockIndex(basic_shader, "TransformBlock") << std::endl;

					// basic_shader.use();
					// camera.set_uniform(basic_shader);
					// basic_shader.set("n_lights", (unsigned int)lights.size());
					
					// ground->draw();
					// backpack->draw();

					// rigging_shader.use();
					// camera.set_uniform(rigging_shader);
					// rigging_shader.set("n_lights", (unsigned int)lights.size());
					
					// walking_man->draw(0, time);
					// dragon->draw(0, time);
					time += delta_time;
					tp += delta_time;
					std::this_thread::sleep_until(tp);
				}
				renderer.terminate = true;
			}
		);

		renderer.execute_renderer(window);
		t.join();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

