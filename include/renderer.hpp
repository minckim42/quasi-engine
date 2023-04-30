#pragma once
#include <vector>
#include "model.hpp"
#include "camera.hpp"
#include "light.hpp"

template <typename ObjectType>
struct RenderRequest
{
	std::shared_ptr<ObjectType> object;
	glm::mat4 transform = glm::mat4(1.0f);
	int idx_animation = 0;
	Microseconds time = 0us;
};

class RendererBase
{
public:
	Camera camera;
	Camera light_camera;
	std::vector<PointLight> point_lights;
	std::vector<RenderRequest<Model>> render_requests;
	Framebuffer shadow_framebuffer;
	Texture shadow_depthbuffer;
	glm::vec2 depth_buffer_size = {10000, 10000};

	void init_framebuffer(int width, int height)
	{
		shadow_depthbuffer.allocate(width, height, Texture::FORMAT::DEPTH_COMPONENT);
		shadow_framebuffer.attach_depthbuffer(shadow_depthbuffer);
		glNamedFramebufferDrawBuffer(shadow_framebuffer, GL_NONE);
		glNamedFramebufferReadBuffer(shadow_framebuffer, GL_NONE);
		if (!shadow_framebuffer.check_status()) 
		{
			throw std::runtime_error("[ERROR] Failed to initialize Shadowbuffer");
		}
	}

	RendererBase():
		transform_matrices(150)
	{
		init_framebuffer(depth_buffer_size.x, depth_buffer_size.y);
	}

	void render()
	{
		shadow_framebuffer.bind();
		glViewport(0, 0, depth_buffer_size.x, depth_buffer_size.y);
		light_camera = Camera(
			point_lights[0].position, -point_lights[0].position, UNIT_Z, {depth_buffer_size.x, depth_buffer_size.y}, 100, 1000000);
		light_camera.update_all();
		glm::vec4 clear_depth(1.0f);
		glClearNamedFramebufferfv(shadow_framebuffer, GL_DEPTH, 0, glm::value_ptr(clear_depth));
		
		// Texture color_buffer;
		// color_buffer.allocate(depth_buffer_size.x, depth_buffer_size.y);
		// shadow_framebuffer.attach_colorbuffer(color_buffer);

		for (auto& request: render_requests)
		{
			draw_shadow(request);
		}

		// color_buffer.save_as_image(root_path / "./result.png");
		// exit(0);
		// return;

		Framebuffer::bind_default();

		glViewport(0, 0, 1280, 720);
		camera.update_all();
		point_lights_buffer.send_to_device(point_lights);
		for (auto& request: render_requests)
		{
			draw(request);
		}
	}

	void draw(RenderRequest<Model>& render_request)
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

	void draw(RenderRequest<Mesh>& render_request)
	{
		std::shared_ptr<Mesh> mesh = render_request.object;
		// Shader* shader = mesh->shader;
		Shader* shader = mesh->shadow_shader;
		shader->use();
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

	void draw_shadow(RenderRequest<Model>& render_request)
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

	void draw_shadow(RenderRequest<Mesh>& render_request)
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

private:
	std::vector<glm::mat4> transform_matrices;
	Buffer<glm::mat4> transform_matrices_buffer;
	Buffer<PointLight> point_lights_buffer;
};




/*
#pragma once
#include <vector>
#include "model.hpp"
#include "camera.hpp"
#include "light.hpp"

template <typename ObjectType>
struct RenderRequest
{
	std::shared_ptr<ObjectType> object;
	glm::mat4 transform = glm::mat4(1.0f);
	int idx_animation = 0;
	Microseconds time = 0us;
};

class RendererBase
{
public:
	Camera camera;
	Camera light_camera;
	std::vector<PointLight> point_lights;
	std::vector<RenderRequest<Model>> render_requests;
	Framebuffer shadow_framebuffer;
	Texture shadow_depthbuffer;

	void init_framebuffer(int width, int height)
	{
		shadow_depthbuffer.allocate(width, height, Texture::FORMAT::DEPTH_COMPONENT);
		shadow_framebuffer.attach_depthbuffer(shadow_depthbuffer);
		glNamedFramebufferDrawBuffer(shadow_framebuffer, GL_NONE);
		glNamedFramebufferReadBuffer(shadow_framebuffer, GL_NONE);
		if (!shadow_framebuffer.check_status()) 
		{
			throw std::runtime_error("[ERROR] Failed to initialize Shadowbuffer");
		}
	}

	RendererBase():
		transform_matrices(150)
	{
		init_framebuffer(12800, 7200);
	}

	void push_request(const RenderRequest<Model>& request)
	{
		render_requests.emplace_back(request);
	}

	void render()
	{
		shadow_framebuffer.bind();
		glViewport(0, 0, 12800, 7200);
		light_camera = Camera(point_lights[0].position, -point_lights[0].position, UNIT_Z, {12800, 7200}, 100, 1000000);
		light_camera.update_all();
		glm::vec4 clear_depth(1.0f);
		glClearNamedFramebufferfv(shadow_framebuffer, GL_DEPTH, 0, glm::value_ptr(clear_depth));
		
		for (auto& request: render_requests)
		{
			draw_shadow(request);
		}

		Framebuffer::bind_default();

		glViewport(0, 0, 1280, 720);
		camera.update_all();
		point_lights_buffer.send_to_device(point_lights);
		for (auto& request: render_requests)
		{
			draw(request);
		}

	}

	void draw(RenderRequest<Model>& render_request)
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

	void draw(RenderRequest<Mesh>& render_request)
	{
		std::shared_ptr<Mesh> mesh = render_request.object;
		Shader* shader = mesh->shadow_shader;
		shader->use();
		
		shadow_depthbuffer.bind(*shader, "shadow_map", 4);
		shader->set("light_view", light_camera.view);
		shader->set("light_projection", light_camera.projection);
		
		
		shader->set("model", render_request.transform);
		camera.set_uniform(*shader);
		point_lights_buffer.send_to_device(point_lights, DRAW_USAGE::DYNAMIC_DRAW);
		point_lights_buffer.bind_as_unifrom(2);
		shader->set("n_lights", (unsigned int)point_lights.size());
		if (mesh->has_bone)
		{
			mesh->bone.update(transform_matrices, render_request.idx_animation, render_request.time);
			transform_matrices_buffer.send_to_device(transform_matrices, DRAW_USAGE::DYNAMIC_DRAW);
			transform_matrices_buffer.bind_as_unifrom(1);
		}
		if (mesh->material)
		{
			mesh->material->bind(*shader);
		}
		mesh->draw();
	}

	void draw_shadow(RenderRequest<Model>& render_request)
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

	void draw_shadow(RenderRequest<Mesh>& render_request)
	{
		std::shared_ptr<Mesh> mesh = render_request.object;
		Shader* shader = mesh->shadow_map_shader;
		shader->use();
		shader->set("model", render_request.transform);
		light_camera.set_uniform(*shader);
		if (mesh->has_bone)
		{
			mesh->bone.update(transform_matrices, render_request.idx_animation, render_request.time);
			transform_matrices_buffer.send_to_device(transform_matrices, DRAW_USAGE::DYNAMIC_DRAW);
			transform_matrices_buffer.bind_as_unifrom(1);
		}
		mesh->draw();
	}

private:
	std::vector<glm::mat4> transform_matrices;
	Buffer<glm::mat4> transform_matrices_buffer;
	Buffer<PointLight> point_lights_buffer;
};


// class Renderer
// {
// public:
// 	void render(Microseconds timebase)
// 	{
// 		Clock::time_point t0;
// 		while (!stop)
// 		{
// 			renderer_base[NOW].render();
// 			t0 += timebase;
// 			swap_renderer_base();
// 			flush_requset();
// 			std::this_thread::sleep_until(t0);
// 		}
// 	}

// 	void push_request(const RenderRequest<Model>& request)
// 	{
// 		renderer_base[BACKGROUND].push_request(request);
// 	}

// 	void flush_requset()
// 	{

// 	}

// 	void swap_renderer_base()
// 	{
// 		std::swap(renderer_base[0], renderer_base[1]);
// 	}

// private:
// 	enum
// 	{
// 		NOW,
// 		BACKGROUND,
// 	};

// 	bool stop = false;
// 	RendererBase renderer_base[2];
// };


*/