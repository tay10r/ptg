#pragma once

#include <glm/glm.hpp>

namespace ptg {

class texture;

class kernel
{
public:
  kernel() = default;

  kernel(const kernel&) = default;

  kernel(kernel&&) = default;

  kernel& operator=(const kernel&) = default;

  kernel& operator=(kernel&&) = default;

  virtual ~kernel() = default;

  virtual void dispatch(glm::uvec2 work_group_count) = 0;

  virtual int get_uniform_location(const char* name) = 0;

  virtual void set_uniform_uint(int location, unsigned int value) = 0;

  virtual void set_uniform_uvec2(int location, glm::uvec2 value) = 0;

  virtual void set_uniform_uvec3(int location, glm::uvec3 value) = 0;

  virtual void set_uniform_uvec4(int location, glm::uvec4 value) = 0;

  virtual void set_uniform_int(int location, int value) = 0;

  virtual void set_uniform_ivec2(int location, glm::ivec2 value) = 0;

  virtual void set_uniform_ivec3(int location, glm::ivec3 value) = 0;

  virtual void set_uniform_ivec4(int location, glm::ivec4 value) = 0;

  virtual void set_uniform_float(int location, float value) = 0;

  virtual void set_uniform_vec2(int location, const glm::vec2 value) = 0;

  virtual void set_uniform_vec3(int location, const glm::vec3 value) = 0;

  virtual void set_uniform_vec4(int location, const glm::vec4 value) = 0;

  virtual void set_active_texture(int texture_index, texture*) = 0;
};

} // namespace ptg
