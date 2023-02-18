#include "render_kernel.hpp"

#include <glm/glm.hpp>

#include <glm/gtx/transform.hpp>

#include "../texture.hpp"

namespace ptg {

render_kernel::render_kernel()
{
  register_uniform("pixel_coordinate", &pixel_coordinate_);

  register_uniform("unit_sphere_sample", &unit_sphere_sample_);

  register_uniform("camera_position", &camera_position_);

  register_uniform("camera_rotation", &camera_rotation_);

  register_uniform("previous_texture", &previous_texture_);

  register_uniform("next_texture", &next_texture_);

  register_uniform("rock_texture", &rock_texture_);

  register_uniform("soil_texture", &soil_texture_);
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

      const auto dx = u * 2 - 1;
      const auto dy = v * 2 - 1;

      const auto ray_dir = glm::normalize(dir + (up * dy) + (right * dx));

      const auto r = ray{ camera_position_, ray_dir };

      const auto color = trace(r);

      const auto texel_index = y * image_bounds.x + x;

      next_texture[texel_index] = glm::vec4(color, 1.0f) + previous_texture[texel_index];
    }
  }
}

glm::vec3
render_kernel::trace(const ray& r) const
{
  const auto h = find_closest_hit(r);

  if (h.distance >= far_)
    return on_miss(r.org, r.dir);

  constexpr auto shadow_bias = 1e-3f;

  const auto second_ray_dir = unit_sphere_sample_ * glm::sign(dot(h.normal, unit_sphere_sample_));

  const auto second_ray_org = r.org + r.dir * (h.distance - shadow_bias);

  const auto second_ray = ray{ second_ray_org, second_ray_dir };

  if (find_any_hit(second_ray))
    return { 0, 0, 0 };

  return glm::vec3(0, 1, 0) * on_miss(second_ray.org, second_ray.dir);
}

struct scene final
{
  glm::vec3 q00{ -1, 0.5 + 9.0, 0 - 3 };
  glm::vec3 q01{ -1, 0.2 + 9.0, 1 - 3 };
  glm::vec3 q10{ 1, 0.1 + 9.0, 0 - 3 };
  glm::vec3 q11{ 1, 0.7 + 9.0, 1 - 3 };

  static const scene& get()
  {
    static const scene s;
    return s;
  }
};

render_kernel::hit
render_kernel::find_closest_hit(const ray& r) const
{
  const auto& s = scene::get();

  return intersect_patch(r, s.q00, s.q01, s.q10, s.q11);
}

bool
render_kernel::find_any_hit(const ray& r) const
{
  const auto& s = scene::get();

  return intersect_patch(r, s.q00, s.q01, s.q10, s.q11).distance < far_;
}

render_kernel::hit
render_kernel::intersect_patch(const ray& r,
                               glm::vec3 q00,
                               glm::vec3 q01,
                               glm::vec3 q10,
                               glm::vec3 q11) const
{
  const auto e10 = q10 - q00;
  const auto e11 = q11 - q10;
  const auto e00 = q01 - q00;

  const auto qn = cross(e10, q01 - q11);

  q00 -= r.org;
  q10 -= r.org;

  const auto a = dot(cross(q00, r.dir), e00);
  const auto c = dot(qn, r.dir);
  auto b = dot(cross(q10, r.dir), e11);

  b -= a + c;

  float det = b * b - 4 * a * c;

  if (det < 0)
    return hit{ far_ };

  det = sqrt(det);

  float u1{ 0.0f };
  float u2{ 0.0f };

  float u{ 0.0f };
  float v{ 0.0f };

  float t = far_;

  if (c == 0.0f) {
    u1 = -a / b;
    u2 = -1;
  } else {
    u1 = (-b - copysignf(det, b)) / 2.0f;
    u2 = a / u1;
    u1 /= c;
  }

  if (0.0f <= u1 && u1 <= 1.0f) {
    auto pa = mix(q00, q10, u1);
    auto pb = mix(e00, e11, u1);
    auto n = cross(r.dir, pb);
    det = dot(n, n);
    n = cross(n, pa);
    auto t1 = dot(n, pb);
    auto v1 = dot(n, r.dir);
    if (t1 > 0.0f && 0.0f <= v1 && v1 < det) {
      t = t1 / det;
      u = u1;
      v = v1 / det;
    }
  }

  if (0.0f <= u2 && u1 <= 1.0f) {
    auto pa = mix(q00, q10, u2);
    auto pb = mix(e00, e11, u2);
    auto n = cross(r.dir, pb);
    det = dot(n, n);
    n = cross(n, pa);
    auto t2 = dot(n, pb) / det;
    auto v2 = dot(n, r.dir);
    if (0.0f <= v2 && v2 <= det && t > t2 && t2 > 0) {
      t = t2;
      u = u2;
      v = v / det;
    }
  }

  if (t >= near_ && t < far_) {
    auto du = mix(e10, q11 - q01, v);
    auto dv = mix(e00, e11, u);

    // Note: This normal is not a unit vector.
    auto n = cross(du, dv);

    return { t, n };
  }

  return hit{ far_ };
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
