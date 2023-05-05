#pragma once
#include <glm/glm.hpp>
#include "move.hpp"
#include "opengl/shader.hpp"

class Camera: public Move
{
public:
   Camera() = default;
   Camera(
      const glm::vec3& position,
      const glm::vec3& direction,
      const glm::vec3& up,
      const glm::vec2& screen_size,
      float near,
      float far
   );
   void update_all() noexcept;
   void update_view() noexcept;
   void update_projection() noexcept;
   void set_uniform(const Shader& shader) const noexcept;

   void set_viewport() const;

   glm::mat4 view;
   glm::mat4 projection;
   glm::vec2 screen_size;
   float near;
   float far;
   float fovy;
};

std::ostream& operator<<(std::ostream& os, Camera& camera);