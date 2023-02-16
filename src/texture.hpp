#pragma once

namespace ptg {

/// @brief The interface for a texture.
class texture
{
public:
  texture() = default;

  texture(const texture&) = delete;

  texture(texture&&) = default;

  texture& operator=(const texture&) = delete;

  texture& operator=(texture&&) = delete;

  virtual ~texture() = default;

  virtual void read_data(float* data) = 0;

  virtual uint32_t get_size() const = 0;
};

} // namespace ptg
