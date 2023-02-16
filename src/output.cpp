#include "output.hpp"

#include <vector>

namespace ptg {

output::output(std::shared_ptr<device> dev, const uint32_t terrain_size)
  : device_(dev)
  , terrain_size_(terrain_size)
  , rock_height_(dev->create_texture(terrain_size))
  , soil_height_(dev->create_texture(terrain_size))
{
}

output::~output()
{
  //
}

bool
output::save_height_png(const char* path, ptg_write_png png_writer)
{
  const uint32_t total_size = terrain_size_ * terrain_size_;

  std::vector<float> hdr_result(total_size, 0.0f);

  {
    std::vector<float> rock_data(total_size);

    rock_height_->read_data(rock_data.data());

    for (uint32_t i = 0; i < total_size; i++)
      hdr_result[i] += rock_data[i];
  }

  {
    std::vector<float> soil_data(total_size);

    soil_height_->read_data(soil_data.data());

    for (uint32_t i = 0; i < total_size; i++)
      hdr_result[i] += soil_data[i];
  }

  float min_h = hdr_result[0];
  float max_h = hdr_result[0];

  for (uint32_t i = 1; i < total_size; i++) {
    min_h = (min_h < hdr_result[i]) ? min_h : hdr_result[i];
    max_h = (max_h > hdr_result[i]) ? max_h : hdr_result[i];
  }

  std::vector<uint8_t> ldr_result(total_size);

  auto clamp = [](int x) -> uint8_t {
    x = (x < 0) ? 0 : x;
    x = (x > 255) ? 255 : x;
    return x & 0xff;
  };

  const float scale = 255.0f / (max_h - min_h);

  for (uint32_t i = 0; i < total_size; i++)
    ldr_result[i] = clamp((hdr_result[i] - min_h) * scale);

  return png_writer(path, terrain_size_, terrain_size_, 1, ldr_result.data(), terrain_size_);
}

uint32_t
output::prepare_bake(const model& m)
{
  if (!bake_job_.has_value())
    return 0;

  auto m_copy = m.copy_current_memento();

  bake_job_ = bake_job{ std::move(m_copy) };
}

bool
output::iterate_bake()
{
  if (!bake_job_)
    return false;

  if (bake_job_->operation_index >= bake_job_->m.operations.size())
    return false;

  const auto& op = bake_job_->m.operations[bake_job_->operation_index];

  switch (op.kind) {
    case operation_kind::apply_path:
      bake_job_->m.paths[op.index];
      break;
  }

  bake_job_->operation_index++;

  return true;
}

} // namespace ptg