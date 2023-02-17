#include "render.hpp"

#include "kernel_registry.hpp"
#include "kernel.hpp"
#include "texture.hpp"

#include <vector>

namespace ptg {

render::render(std::shared_ptr<device> dev, const uint32_t image_size)
  : device_(std::move(dev))
    , color_(device_->create_texture(image_size))
{
}

void
render::iterate()
{
  auto* next_texture = device_->create_texture(color_->get_size());

  auto* kern = device_->get_kernel_registry()->render_kernel;

  const auto image_size = color_->get_size();

  const auto previous_texture_location = kern->get_uniform_location("previous_texture");

  const auto next_texture_location = kern->get_uniform_location("next_texture");

  const auto camera_position_location = kern->get_uniform_location("camera_position");

  const auto camera_rotation_location = kern->get_uniform_location("camera_rotation");

  const auto work_group_count = glm::uvec2(image_size, image_size) / device_->get_work_group_size();

  kern->set_active_texture(0, color_);

  kern->set_active_texture(1, next_texture);

  kern->set_uniform_int(previous_texture_location, 0);

  kern->set_uniform_int(next_texture_location, 1);

  kern->set_uniform_vec3(camera_position_location, camera_.position);

  kern->set_uniform_vec3(camera_rotation_location, camera_.rotation);

  kern->dispatch(work_group_count);

  device_->destroy_texture(color_);

  color_ = next_texture;

  samples_per_pixel_++;
}

bool
render::save_to_png(const char* path, ptg_write_png png_writer) const
{
  const auto image_size = color_->get_size();

  std::vector<float> color(image_size * image_size * 4);

  color_->read_data(color.data());

  const float color_weight = 1.0f / static_cast<float>(samples_per_pixel_);

  glm::vec4 max_color{ glm::vec4(color[0], color[1], color[2], color[3]) * color_weight };

  for (uint32_t i = 0; i < color.size(); i += 4) {

    const glm::vec4 c(color[i], color[i + 1], color[i + 2], color[i + 3]);

    max_color = glm::max(max_color, c * color_weight);
  }

  const glm::vec4 color_scale = 255.0f / max_color;

  std::vector<unsigned char> ldr_color(image_size * image_size * 4);

  auto clamp = [](const int x) -> unsigned char {
    if (x < 0)
      return 0;
    if (x > 255)
      return 255;
    return x;
  };

  for (uint32_t i = 0; i < color.size(); i += 4) {
    const float r = color[i + 0] * color_weight;
    const float g = color[i + 1] * color_weight;
    const float b = color[i + 2] * color_weight;
    ldr_color[i + 0] = clamp(static_cast<int>(r * color_scale.r));
    ldr_color[i + 1] = clamp(static_cast<int>(g * color_scale.g));
    ldr_color[i + 2] = clamp(static_cast<int>(b * color_scale.b));
    ldr_color[i + 3] = 255;
  }

  return !!png_writer(path, image_size, image_size, 4, ldr_color.data(), image_size * 4u);
}

} // namespace ptg
