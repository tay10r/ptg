// LEFT OFF:
//   Need to decide if imageStore is okay.
//   Consider testing for stateful invocation of cpu kernel.

#pragma once

#include "kernel.hpp"

#include <vector>
#include <map>
#include <string>

namespace ptg {

/// This is a base class for a CPU kernel.
class cpu_kernel : public kernel
{
public:
  static constexpr glm::uvec2 work_group_size() { return { 4u, 4u }; };

  cpu_kernel() = default;

  cpu_kernel(const cpu_kernel&) = default;

  cpu_kernel(cpu_kernel&&) = default;

  cpu_kernel& operator=(const cpu_kernel&) = delete;

  cpu_kernel& operator=(cpu_kernel&&) = delete;

  ~cpu_kernel() override = default;

  void dispatch(glm::uvec2 work_group_count) override;

  virtual void local_dispatch(glm::uvec2 work_group_id, glm::uvec2 work_group_count) = 0;

  void register_uniform(const char* name, void* ptr);

  void set_active_texture(int texture_index, texture*) override;

  int get_uniform_location(const char* name) override;

  void set_uniform_uint(const int location, const unsigned int value) override
  {
    set_generic_uniform(location, value);
  }

  void set_uniform_uvec2(const int location, const glm::uvec2 value) override
  {
    set_generic_uniform(location, value);
  }

  void set_uniform_uvec3(const int location, const glm::uvec3 value) override
  {
    set_generic_uniform(location, value);
  }

  void set_uniform_uvec4(const int location, const glm::uvec4 value) override
  {
    set_generic_uniform(location, value);
  }

  void set_uniform_int(const int location, const int value) override
  {
    set_generic_uniform(location, value);
  }

  void set_uniform_ivec2(const int location, const glm::ivec2 value) override
  {
    set_generic_uniform(location, value);
  }

  void set_uniform_ivec3(const int location, const glm::ivec3 value) override
  {
    set_generic_uniform(location, value);
  }

  void set_uniform_ivec4(const int location, const glm::ivec4 value) override
  {
    set_generic_uniform(location, value);
  }

  void set_uniform_float(const int location, const float value) override
  {
    set_generic_uniform(location, value);
  }

  void set_uniform_vec2(const int location, const glm::vec2 value) override
  {
    set_generic_uniform(location, value);
  }

  void set_uniform_vec3(const int location, const glm::vec3 value) override
  {
    set_generic_uniform(location, value);
  }

  void set_uniform_vec4(const int location, const glm::vec4 value) override
  {
    set_generic_uniform(location, value);
  }

protected:
  texture* get_texture(const int texture_index) { return active_textures_[texture_index]; }

  [[nodiscard]] const texture* get_texture(const int texture_index) const { return active_textures_[texture_index]; }

private:
  template<typename T>
  void set_generic_uniform(const int location, const T value)
  {
    void* ptr = uniform_pointers_.at(location);

    *static_cast<T*>(ptr) = value;
  }

private:
  std::map<std::string, int> uniform_locations_;

  std::vector<void*> uniform_pointers_;

  std::vector<texture*> active_textures_{ static_cast<std::vector<texture*>::size_type>(4), nullptr };
};

} // namespace ptg
