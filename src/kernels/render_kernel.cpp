#include "render_kernel.hpp"

#include <glm/gtx/transform.hpp>

#include "../texture.hpp"

namespace ptg {

render_kernel::render_kernel()
{
  register_uniform("pixel_coordinate", &pixel_coordinate_);

  register_uniform("hemisphere", &hemisphere_);

  register_uniform("camera_position", &camera_position_);

  register_uniform("camera_rotation", &camera_rotation_);

  register_uniform("previous_texture", &previous_texture_);

  register_uniform("next_texture", &next_texture_);
}

void
render_kernel::local_dispatch(const glm::uvec2 work_group_id, const glm::uvec2 work_group_count)
{
  const auto* previous_texture = static_cast<const glm::vec4*>(get_texture(previous_texture_)->get_data_pointer());

  auto* next_texture = static_cast<glm::vec4*>(get_texture(next_texture_)->get_data_pointer());

  const auto p_min = (work_group_id + glm::uvec2(0, 0)) * work_group_size();
  const auto p_max = (work_group_id + glm::uvec2(1, 1)) * work_group_size();
  const auto image_bounds = work_group_count * work_group_size();

  const auto x_scale = 1.0f / static_cast<float>(image_bounds.x);
  const auto y_scale = 1.0f / static_cast<float>(image_bounds.y);

  const glm::mat3 rotation = rotate(glm::radians(camera_rotation_.z), glm::vec3(0, 0, 1)) *
                             rotate(glm::radians(camera_rotation_.y), glm::vec3(0, 1, 0)) *
                             rotate(glm::radians(camera_rotation_.x), glm::vec3(1, 0, 0));

  const auto dir = normalize(rotation * glm::vec3(0, -1, 0));

  const auto up = normalize(rotation * glm::vec3(0, 0, 1));

  const auto right = normalize(rotation * glm::vec3(1, 0, 0));

  for (uint32_t y = p_min.y; y < p_max.y; y++) {

    for (uint32_t x = p_min.x; x < p_max.x; x++) {

      const auto u = (static_cast<float>(x) + pixel_coordinate_.x) * x_scale;
      const auto v = (static_cast<float>(y) + pixel_coordinate_.y) * y_scale;

      const auto ray_org = camera_position_;
      const auto ray_dir = glm::normalize(dir + (up * v) + (right * u));

      const auto color = on_miss(ray_org, ray_dir);

      const auto texel_index = y * image_bounds.x + x;

      next_texture[texel_index] = glm::vec4(color, 1.0f) + previous_texture[texel_index];
    }
  }
}

glm::vec3
render_kernel::on_miss(const glm::vec3& ray_org, const glm::vec3& ray_dir) const
{
  const glm::vec3 up(0, 1, 0);

  const float level = (glm::dot(up, ray_dir) + 1.0f) * 0.5f;

  const glm::vec3 lo(1.0, 1.0, 1.0);
  const glm::vec3 hi(0.5, 0.7, 1.0);

  return lo + ((hi - lo) * level);
}

} // namespace ptg
