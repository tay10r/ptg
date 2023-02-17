#pragma once

#include "../cpu_kernel.hpp"

namespace ptg {

class render_kernel final : public cpu_kernel
{
public:
  render_kernel();

  void local_dispatch(glm::uvec2 work_group_id, glm::uvec2 work_group_count) override;

private:
  /// @brief Called when the ray does not intersect the terrain.
  ///
  /// @param ray_org The origin point of the ray.
  ///
  /// @param ray_dir The direction the ray is pointing.
  [[nodiscard]] glm::vec3 on_miss(const glm::vec3& ray_org, const glm::vec3& ray_dir) const;

  /// @brief The index of the texture containing the previous results.
  int previous_texture_{ -1 };

  /// @brief The index of the texture to write the results to.
  int next_texture_{ -1 };

  /// @brief The point on the hemisphere being sampled.
  glm::vec3 hemisphere_{ 0, 1, 0 };

  /// @brief The pixel coordinate (coordinate within a pixel) to cast the ray from.
  glm::vec2 pixel_coordinate_{ 0.5f, 0.5f };

  /// @brief The position at which the camera is at.
  glm::vec3 camera_position_{ 0, 0, 0 };

  /// @brief The rotation to apply to the camera direction.
  ///        By default, the camera is looking down.
  glm::vec3 camera_rotation_{ 0, 0, 0 };
};

} // namespace ptg
