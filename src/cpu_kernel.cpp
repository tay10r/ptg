#include "cpu_kernel.hpp"

namespace ptg {

void
cpu_kernel::set_active_texture(const int texture_index, texture* t)
{
  active_textures_.at(texture_index) = t;
}

void
cpu_kernel::register_uniform(const char* name, void* ptr)
{
  uniform_pointers_.emplace_back(ptr);

  const int location = static_cast<int>(uniform_pointers_.size() - 1);

  uniform_locations_.emplace(name, location);
}

int
cpu_kernel::get_uniform_location(const char* name)
{
  auto it = uniform_locations_.find(name);
  if (it == uniform_locations_.end())
    return -1;

  return it->second;
}

void
cpu_kernel::dispatch(const glm::uvec2 work_group_count)
{
  for (uint32_t y = 0; y < work_group_count.y; y++) {
    for (uint32_t x = 0; x < work_group_count.x; x++) {
      local_dispatch({ x, y }, work_group_count);
    }
  }
}

} // namespace ptg
