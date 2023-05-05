#include "renderer.hpp"
#include <vector>
#include "model.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "glfw_window.hpp"


void RendererBase::init_framebuffer(int width, int height)
{
	shadow_depthbuffer.allocate_depth(width, height);
	shadow_framebuffer.attach_depthbuffer(shadow_depthbuffer);
	glNamedFramebufferDrawBuffer(shadow_framebuffer, GL_NONE);
	glNamedFramebufferReadBuffer(shadow_framebuffer, GL_NONE);
	if (!shadow_framebuffer.check_status()) 
	{
		throw std::runtime_error("[ERROR] Failed to initialize Shadowbuffer");
	}
}

RendererBase::RendererBase():
	transform_matrices(150)
{
	init_framebuffer(depth_buffer_size.x, depth_buffer_size.y);
	std::array<std::filesystem::path, 6> path = 
		{
			root_path / "./resource/skybox/right.jpg",
			root_path / "./resource/skybox/left.jpg",
			root_path / "./resource/skybox/top.jpg",
			root_path / "./resource/skybox/bottom.jpg",
			root_path / "./resource/skybox/front.jpg",
			root_path / "./resource/skybox/back.jpg",
		};
	env_map = std::move(Texture::create_cube_map(path));
}

void RendererBase::execute_renderer(Window& window)
{
	glEnable(GL_DEPTH_TEST);
	while (!terminate)
	{
		render(window);
	}
}

void RendererBase::render(Window& window)
{
	TimePoint t0 = Clock::now();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.4f, 0.4f, 0.9f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	if (enable_shadow)
	{
		shadow_framebuffer.bind();
		glViewport(0, 0, depth_buffer_size.x, depth_buffer_size.y);
		light_camera = Camera(
			point_lights[0].position, -point_lights[0].position, UNIT_Z, {depth_buffer_size.x, depth_buffer_size.y}, 100, 1000000);
		light_camera.update_all();
		glm::vec4 clear_depth(1.0f);
		glClearNamedFramebufferfv(shadow_framebuffer, GL_DEPTH, 0, glm::value_ptr(clear_depth));
		for (auto& request: render_requests)
		{
			draw_shadow(request);
		}
		Framebuffer::bind_default();
	}


	camera.set_viewport();
	camera.update_all();
	point_lights_buffer.send_to_device(point_lights);
	for (auto& request: render_requests)
	{
		draw(request);
	}
	glfwSwapBuffers(window);
	glfwPollEvents();

	TimePoint t1 = Clock::now();
	framerate = 1000000000.0f / (t1 - t0).count();
	if (enable_check_framerate)
		std::cout << "framerate: " << framerate << std::endl;
}

void RendererBase::draw(RenderRequest<Model>& render_request)
{
	std::shared_ptr<Model> model = render_request.object;
	RenderRequest<Model> req_model = {
		.transform = render_request.transform * model->transform,
		.idx_animation = render_request.idx_animation,
		.time = render_request.time,
	};
	for (auto& child: model->children)
	{
		req_model.object = child;
		draw(req_model);
	}
	RenderRequest<Mesh> req_mesh = {
		.transform = render_request.transform * model->transform,
		.idx_animation = render_request.idx_animation,
		.time = render_request.time
	};
	for (auto& mesh: model->meshes)
	{
		req_mesh.object = mesh;
		draw(req_mesh);
	}
}

void RendererBase::draw(RenderRequest<Mesh>& render_request)
{
	std::shared_ptr<Mesh> mesh = render_request.object;
	Shader* shader;
	if (enable_shadow)
	{
		if (enable_env)
		{
			shader = mesh->shadow_env_shader;
		}
		else
		{
			shader = mesh->shadow_shader;
		}
	}
	else
	{
		if (enable_env)
		{

		}
		else
		{
			shader = mesh->shader;
		}
	}
	if (enable_transparent)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
	}
	shader->use();
	env_map.bind(
		*shader,
		static_cast<int>(Shader::TEXTURE_LOCATION_BINDING::ENV_MAP),
		static_cast<int>(Shader::TEXTURE_LOCATION_BINDING::ENV_MAP)
	);
	shader->set(static_cast<int>(Shader::UNIFORM_LOCATION::MODEL), render_request.transform);
	shader->set(static_cast<int>(Shader::UNIFORM_LOCATION::SHADOW_VIEW), light_camera.view);
	shader->set(static_cast<int>(Shader::UNIFORM_LOCATION::SHADOW_PROJECTION), light_camera.projection);
	camera.set_uniform(*shader);
	shadow_depthbuffer.bind(
		*shader, 
		static_cast<int>(Shader::TEXTURE_LOCATION_BINDING::SHADOW_MAP), 
		static_cast<int>(Shader::TEXTURE_LOCATION_BINDING::SHADOW_MAP)
	);
	if (mesh->material)
	{
		mesh->material->bind(*shader);
	}
	point_lights_buffer.send_to_device(point_lights, DRAW_USAGE::DYNAMIC_DRAW);
	point_lights_buffer.bind_as_unifrom(2);
	shader->set(static_cast<int>(Shader::UNIFORM_LOCATION::N_LIGHT), (unsigned int)point_lights.size());
	
	if (mesh->has_bone)
	{
		mesh->bone.update(transform_matrices, render_request.idx_animation, render_request.time);
		transform_matrices_buffer.send_to_device(transform_matrices, DRAW_USAGE::DYNAMIC_DRAW);
		transform_matrices_buffer.bind_as_unifrom(1);
	}
	mesh->draw();
}

void RendererBase::draw_shadow(RenderRequest<Model>& render_request)
{
	std::shared_ptr<Model> model = render_request.object;
	RenderRequest<Model> req_model = {
		.transform = render_request.transform * model->transform,
		.idx_animation = render_request.idx_animation,
		.time = render_request.time,
	};
	for (auto& child: model->children)
	{
		req_model.object = child;
		draw_shadow(req_model);
	}

	RenderRequest<Mesh> req_mesh = {
		.transform = render_request.transform * model->transform,
		.idx_animation = render_request.idx_animation,
		.time = render_request.time
	};
	for (auto& mesh: model->meshes)
	{
		req_mesh.object = mesh;
		draw_shadow(req_mesh);
	}
}

void RendererBase::draw_shadow(RenderRequest<Mesh>& render_request)
{
	std::shared_ptr<Mesh> mesh = render_request.object;
	Shader* shader = mesh->shadow_map_shader;
	shader->use();
	shader->set(static_cast<int>(Shader::UNIFORM_LOCATION::MODEL), render_request.transform);
	light_camera.set_uniform(*shader);

	if (mesh->has_bone)
	{
		mesh->bone.update(transform_matrices, render_request.idx_animation, render_request.time);
		transform_matrices_buffer.send_to_device(transform_matrices, DRAW_USAGE::DYNAMIC_DRAW);
		transform_matrices_buffer.bind_as_unifrom(1);
	}
	mesh->draw();
}
