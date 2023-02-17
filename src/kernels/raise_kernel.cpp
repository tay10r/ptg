#include "raise_kernel.hpp"

#include "../texture.hpp"

namespace ptg {

raise_kernel::raise_kernel()
{
  register_uniform("brush_center", &brush_center_);

  register_uniform("brush_size", &brush_size_);

  register_uniform("terrain_texel_size", &terrain_texel_size_);

  register_uniform("input_texture", &input_texture_);

  register_uniform("output_texture", &output_texture_);
}

void
raise_kernel::local_dispatch(const glm::uvec2 work_group_id, const glm::uvec2 work_group_count)
{
  const auto* input = static_cast<const glm::vec4*>(get_texture(input_texture_)->get_data_pointer());

  auto* output = static_cast<glm::vec4*>(get_texture(output_texture_)->get_data_pointer());

  const auto p_min = work_group_id * work_group_size();

  const auto p_max = (work_group_id + glm::uvec2(1, 1)) * work_group_size();

  const glm::vec4 brush_center_x{ brush_center_.x, brush_center_.x, brush_center_.x, brush_center_.x };
  const glm::vec4 brush_center_y{ brush_center_.y, brush_center_.y, brush_center_.y, brush_center_.y };

  const auto texel_bounds = work_group_size() * work_group_count;

  const auto distance_scale = 1.0f / brush_size_;

  for (uint32_t y = p_min.y; y < p_max.y; y++) {

    for (uint32_t x = p_min.x; x < p_max.x; x++) {

      const auto x0 = (x * 2 * terrain_texel_size_) + 0;
      const auto x1 = (x * 2 * terrain_texel_size_) + 1;

      const auto y0 = (y * 2 * terrain_texel_size_) + 0;
      const auto y1 = (y * 2 * terrain_texel_size_) + 1;

      const glm::vec4 pos_x{ x0, x1, x0, x1 };
      const glm::vec4 pos_y{ y0, y0, y1, y1 };

      const auto delta_x = pos_x - brush_center_x;
      const auto delta_y = pos_y - brush_center_y;

      const auto distance = glm::sqrt(delta_x * delta_x + delta_y * delta_y);

      const auto texel_index = (y * texel_bounds.x) + x;

      output[texel_index] = input[texel_index] + glm::vec4(1.0f) / (glm::vec4(1.0f) + distance * distance_scale);
    }
  }
}

} // namespace ptg
