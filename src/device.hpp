#pragma once

#include <glm/glm.hpp>

#include <stdint.h>

#include "ptg.h"

namespace ptg {

class texture;

struct kernel_registry;

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

  /// @brief Releases memory allocated by a texture.
  ///
  /// @param t The texture to release the memory of.
  virtual void destroy_texture(texture* t) = 0;

  /// @brief Creates a copy of a texture.
  ///
  /// @param src The texture to copy.
  ///
  /// @return A pointer to the copied texture.
  virtual texture* copy_texture(texture* src) = 0;

  /// @brief Gets the kernel registry for the device.
  ///
  /// @return A pointer to the kernel registry for the device.
  virtual const kernel_registry* get_kernel_registry() = 0;

  /// @brief Gets the size of a work group.
  ///
  /// @return The size of the work group for this device.
  virtual glm::uvec2 get_work_group_size() = 0;

  virtual void log(PtgSeverity severity, const char* msg) = 0;

  void info(const char* msg) { log(PTG_INFO, msg); }

  void warn(const char* msg) { log(PTG_WARN, msg); }

  void error(const char* msg) { log(PTG_ERROR, msg); }
};

} // namespace ptg
