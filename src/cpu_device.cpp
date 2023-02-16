#include "cpu_device.hpp"

#include "texture.hpp"

#include <vector>

namespace ptg {

namespace {

class cpu_texture final : public texture
{
public:
  explicit cpu_texture(const uint32_t size)
    : size_(size)
    , data_(size * size, 0.0f)
  {
  }

  void read_data(float* data) override
  {
    const uint32_t total_size = size_ * size_;

    for (uint32_t i = 0; i < total_size; i++)
      data[i] = data_[i];
  }

  uint32_t get_size() const { return size_; }

private:
  const uint32_t size_{ 0 };

  std::vector<float> data_;
};

class cpu_device final : public device
{
public:
  uint32_t get_max_texture_size() override { return 65536; }

  texture* create_texture(const uint32_t size) override
  {
    auto t = std::make_unique<cpu_texture>(size);
    textures_.emplace_back(std::move(t));
    return textures_.back().get();
  }

private:
  std::vector<std::unique_ptr<cpu_texture>> textures_;
};

} // namespace

std::shared_ptr<device>
create_cpu_device()
{
  return std::make_shared<cpu_device>();
}

} // namespace ptg
