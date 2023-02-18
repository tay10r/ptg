#pragma once

namespace ptg {

/// @brief The interface for a texture.
class texture
{
public:
  texture() = default;

  texture(const texture&) = default;

  texture(texture&&) = default;

  texture& operator=(const texture&) = delete;

  texture& operator=(texture&&) = delete;

  virtual ~texture() = default;

  /// @brief Reads texel data from the texture.
  ///
  /// @note This function works even when the texture data is not stored in CPU memory.
  virtual void read_data(float* data) = 0;

  /// @brief Gets the size of the texture, in both axes.
  ///
  /// @return The size of the texture, in both axes.
  virtual uint32_t get_size() const = 0;

  /// @brief If possible, returns a pointer to the texture data.
  ///
  /// @return A pointer to the texture data.
  ///         If the implementation does not store texture data in CPU memory, this returns a null pointer.
  virtual void* get_data_pointer() = 0;

  /// @brief If possible, returns a pointer to the texture data.
  ///
  /// @return A pointer to the texture data.
  ///         If the implementation does not store texture data in CPU memory, this returns a null pointer.
  virtual const void* get_data_pointer() const = 0;
};

} // namespace ptg
