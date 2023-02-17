#pragma once

#include "device.hpp"

#include <memory>

namespace ptg {

struct camera final
{
  glm::vec3 position{ 0, 0, 0 };

  glm::vec3 rotation{ 0, 0, 0 };

  float aspect{ 1.0f };

  float fov{ 45.0f };

  float near{ 0.01f };

  float far{ 2000.0f };
};

class render final
{
public:
  render(std::shared_ptr<device> dev, uint32_t image_size);

  camera* get_camera() { return &camera_; }

  [[nodiscard]] const camera* get_camera() const { return &camera_; }

  /// @brief Renders one sample per pixel.
  void iterate();

  /// @brief Saves the render to a PNG file.
  ///
  /// @param path The path to save the file at.
  ///
  /// @param png_writer Used to write the result of the render to a PNG file.
  ///
  /// @return True on success, false on failure.
  bool save_to_png(const char* path, ptg_write_png png_writer) const;

private:
  std::shared_ptr<device> device_;

  texture* color_{ nullptr };

  camera camera_;

  /// @brief The number of samples that have been rendered per pixel.
  int samples_per_pixel_{ 0 };
};

} // namespace ptg
