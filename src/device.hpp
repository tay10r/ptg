#pragma once

#include <stdint.h>

namespace ptg {

class texture;

class device
{
public:
  device() = default;

  device(const device&) = delete;

  device(device&&) = delete;

  device& operator=(const device&) = delete;

  device& operator=(device&&) = delete;

  virtual ~device() = default;

  /// @brief Gets the maximum size of a texture.
  ///
  /// @return The maximum size of a texture, in either axis.
  virtual uint32_t get_max_texture_size() = 0;

  /// @brief Creates a new texture.
  ///
  /// @param texture_size The size of the texture being made.
  ///
  /// @return A new texture instance.
  virtual texture* create_texture(uint32_t texture_size) = 0;
};

} // namespace ptg
