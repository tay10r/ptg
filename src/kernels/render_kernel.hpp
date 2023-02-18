#pragma once

#include "../cpu_kernel.hpp"

namespace ptg {

class render_kernel final : public cpu_kernel
{
public:
  render_kernel();

  void local_dispatch(glm::uvec2 work_group_id, glm::uvec2 work_group_count) override;

private:
  /// @brief Stores data associated with a ray.
  struct ray final
  {
    /// @brief The starting point of the ray.
    glm::vec3 org;

    /// @brief The direction that the ray is oriented towards.
    glm::vec3 dir;
  };

  /// @brief Represents a potential intersection between a ray and a patch.
  struct hit final
  {
    /// @brief The distance between the ray and batch (assuming the ray direction is normalized).
    ///        If the ray misses a patch, then this field is not modified.
    ///        Consider assigning it to "tfar" and checking for equality with "tfar" to check for intersection.
    float distance;

    /// @brief The normal at the point of intersection.
    ///        Do not assume this is a unit vector.
    glm::vec3 normal{ 0, 1, 0 };
  };

  /// @brief Computes the color returned by a single ray.
  ///
  /// @parma r The ray to trace.
  ///
  /// @return The color returned by a single ray.
  [[nodiscard]] glm::vec3 trace(const ray& r) const;

  /// @brief Searches for the closest intersection between the ray and the terrain.
  ///
  /// @param r The ray to check intersection for.
  ///
  /// @return A hit structure to indicate if the ray hit the terrain and at what normal.
  [[nodiscard]] hit find_closest_hit(const ray& r) const;

  /// @brief Checks if a ray intersects any part of the terrain.
  ///
  /// @param r The ray to check intersection for.
  ///
  /// @return True if the ray hits the terrain, false otherwise.
  [[nodiscard]] bool find_any_hit(const ray& r) const;

  /// @brief Computes the intersection between a ray and a patch.
  ///
  /// @param r The ray to compute the intersection of.
  ///
  /// @param q00 The first point in the patch.
  ///
  /// @param q01 The second point in the patch.
  ///
  /// @param q10 The third point in the patch.
  ///
  /// @param q11 The fourth point in the patch.
  ///
  /// @return Information regarding the intersection.
  ///         If the ray does not intersect, then the hit distance is assigned render_kernel::far_.
  [[nodiscard]] hit intersect_patch(const ray& r,
                                    glm::vec3 q00,
                                    glm::vec3 q01,
                                    glm::vec3 q10,
                                    glm::vec3 q11) const;

  /// @brief Called when the ray does not intersect the terrain.
  ///
  /// @param ray_org The origin point of the ray.
  ///
  /// @param ray_dir The direction the ray is pointing.
  ///
  /// @return The color of the background, where the ray is pointing at.
  [[nodiscard]] glm::vec3 on_miss(const glm::vec3& ray_org, const glm::vec3& ray_dir) const;

  /// @brief The texture containing the rock layer height.
  int rock_texture_{ -1 };

  /// @brief The texture containing the soil layer height.
  int soil_texture_{ -1 };

  /// @brief The index of the texture containing the previous results.
  int previous_texture_{ -1 };

  /// @brief The index of the texture to write the results to.
  int next_texture_{ -1 };

  /// @brief The point on the hemisphere being sampled.
  glm::vec3 unit_sphere_sample_{ 0, 1, 0 };

  /// @brief The pixel coordinate (coordinate within a pixel) to cast the ray from.
  glm::vec2 pixel_coordinate_{ 0.5f, 0.5f };

  /// @brief The position at which the camera is at.
  glm::vec3 camera_position_{ 0, 0, 0 };

  /// @brief The rotation to apply to the camera direction.
  ///        By default, the camera is looking down.
  glm::vec3 camera_rotation_{ 0, 0, 0 };

  /// @brief The minimum distance allowed for a ray intersection.
  const float near_{ 0.01f };

  /// @brief The maximum distance allowed for a ray intersection.
  const float far_{ 2000.0f };
};

} // namespace ptg
