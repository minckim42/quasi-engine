#pragma once
#include <vector>
#include "opengl/framebuffer.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "glfw_window.hpp"

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
	Texture env_map;
	bool terminate = false;

	bool enable_shadow = false;
	bool enable_env = false;
	bool enable_transparent = true;
	bool enable_check_framerate = false;

	RendererBase();


	void execute_renderer(Window& window);
	void render(Window& window);
	void draw(RenderRequest<Model>& render_request);
	void draw(RenderRequest<Mesh>& render_request);
	void draw_shadow(RenderRequest<Model>& render_request);
	void draw_shadow(RenderRequest<Mesh>& render_request);

private:
	std::vector<glm::mat4> transform_matrices;
	Buffer<glm::mat4> transform_matrices_buffer;
	Buffer<PointLight> point_lights_buffer;
	
	float framerate = 0.0f;


	void init_framebuffer(int width, int height);
};
