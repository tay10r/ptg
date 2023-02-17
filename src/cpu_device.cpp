#include "cpu_device.hpp"

#include "texture.hpp"
#include "kernel_registry.hpp"

#include "kernels/raise.hpp"

#include <vector>

namespace ptg {

namespace {

class cpu_texture final : public texture
{
public:
  explicit cpu_texture(const uint32_t size)
    : size_(size)
      , data_(size * size / 4, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f))
  {
  }

  void read_data(float* data) override
  {
    const uint32_t size = size_ / 2;

    for (uint32_t y = 0; y < size; y++) {

      for (uint32_t x = 0; x < size; x++) {

        const glm::vec4 t = data_[(y * size) + x];

        data[(((y * 2) + 0) * size_) + (x * 2) + 0] = t.x;
        data[(((y * 2) + 0) * size_) + (x * 2) + 1] = t.y;
        data[(((y * 2) + 1) * size_) + (x * 2) + 0] = t.z;
        data[(((y * 2) + 1) * size_) + (x * 2) + 1] = t.w;
      }
    }
  }

  [[nodiscard]] uint32_t get_size() const override { return size_; }

  void* get_data_pointer() override { return data_.data(); }

  [[nodiscard]] const void* get_data_pointer() const override { return data_.data(); }

private:
  const uint32_t size_{ 0 };

  std::vector<glm::vec4> data_;
};

class cpu_device final : public device
{
public:
  cpu_device(void* logger_data, ptg_log_callback logger_func)
    : logger_data_(logger_data)
      , logger_func_(logger_func)
  {
  }

  uint32_t get_max_texture_size() override { return 65536; }

  texture* create_texture(const uint32_t size) override
  {
    auto t = std::make_unique<cpu_texture>(size);
    textures_.emplace_back(std::move(t));
    return textures_.back().get();
  }

  void destroy_texture(texture* t) override
  {
    for (auto it = textures_.begin(); it != textures_.end(); ++it) {
      if (it->get() == t) {
        textures_.erase(it);
        return;
      }
    }
  }

  const kernel_registry* get_kernel_registry() override
  {
    return &kernel_registry_;
  }

  glm::uvec2 get_work_group_size() override
  {
    return cpu_kernel::work_group_size();
  }

  void log(PtgSeverity severity, const char* msg) override
  {
    if (logger_func_)
      logger_func_(logger_data_, severity, msg);
  }

private:
  std::vector<std::unique_ptr<cpu_texture>> textures_;

  raise_kernel raise_kernel_;

  const kernel_registry kernel_registry_{ &raise_kernel_ };

  void* logger_data_{ nullptr };

  ptg_log_callback logger_func_{ nullptr };
};

} // namespace

std::shared_ptr<device>
create_cpu_device(void* logger_data, ptg_log_callback logger_func)
{
  return std::make_shared<cpu_device>(logger_data, logger_func);
}

} // namespace ptg
