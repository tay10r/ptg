#pragma once

#include "../cpu_kernel.hpp"

namespace ptg {

class raise_kernel final : public cpu_kernel
{
public:
  raise_kernel();

  void local_dispatch(const glm::uvec2 work_group_id, const glm::uvec2 work_group_count) override;

private:
  float terrain_texel_size_{ 1 };

  glm::vec2 brush_center_{ 0, 0 };

  float brush_size_{ 1 };

  int input_texture_{ -1 };

  int output_texture_{ -1 };
};

} // namespace ptg
