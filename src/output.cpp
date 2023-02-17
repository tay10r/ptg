#include "output.hpp"

#include <vector>

#include "kernel.hpp"
#include "kernel_registry.hpp"

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
  if (bake_job_) {
    device_->error("Cannot prepare bake because one is already active.");
    return 0u;
  }

  auto m_copy = m.copy_current_memento();

  const auto op_count = m_copy.operations.size();

  bake_job_ = bake_job{ std::move(m_copy) };

  return op_count;
}

bool
output::iterate_bake()
{
  if (!bake_job_) {
    device_->error("Cannot iterate bake job because one does not exist.");
    return false;
  }

  const auto& op = bake_job_->m.operations[bake_job_->operation_index];

  switch (op.kind) {
    case operation_kind::apply_path:
      apply_raise_operation(bake_job_->m.paths[op.index]);
      break;
  }

  bake_job_->operation_index++;

  if (bake_job_->operation_index >= bake_job_->m.operations.size()) {
    device_->info("Bake operation complete.");
    bake_job_.reset();
  }

  return true;
}

void
output::apply_raise_operation(const path& p)
{
  auto* k = device_->get_kernel_registry()->raise_kernel;

  const auto work_group_size = device_->get_work_group_size();

  const auto meters_per_axis = bake_job_->m.meters_per_axis;

  const auto brush_center_location = k->get_uniform_location("brush_center");

  const auto brush_size_location = k->get_uniform_location("brush_size");

  const auto terrain_texel_size_location = k->get_uniform_location("terrain_texel_size");

  const auto input_texture_location = k->get_uniform_location("input_texture");

  const auto output_texture_location = k->get_uniform_location("output_texture");

  k->set_uniform_float(terrain_texel_size_location, meters_per_axis / static_cast<float>(terrain_size_));

  k->set_uniform_float(brush_size_location, p.brush_size);

  for (uint32_t i = 0; i < p.xy_coordinates.size(); i += 2) {

    auto* input_texture = get_layer_texture(p.layer);

    auto* output_texture = device_->create_texture(input_texture->get_size());

    k->set_active_texture(0, input_texture);
    k->set_active_texture(1, output_texture);

    k->set_uniform_int(input_texture_location, 0);
    k->set_uniform_int(output_texture_location, 1);

    const auto x = p.xy_coordinates[i + 0];
    const auto y = p.xy_coordinates[i + 1];

    k->set_uniform_vec2(brush_center_location, glm::vec2{ x, y });

    const uint32_t work_group_count_x = terrain_size_ / (work_group_size.x * 2u);
    const uint32_t work_group_count_y = terrain_size_ / (work_group_size.y * 2u);

    k->dispatch(glm::uvec2{ work_group_count_x, work_group_count_y });

    set_layer_texture(p.layer, output_texture);

    device_->destroy_texture(input_texture);
  }
}

texture*
output::get_layer_texture(const PtgLayer layer)
{
  switch (layer) {
    case PTG_LAYER_ROCK:
      return rock_height_;
    case PTG_LAYER_SOIL:
      return soil_height_;
  }

  device_->error("Failed to get layer texture because the layer is not recognized.");

  return nullptr;
}

void
output::set_layer_texture(const PtgLayer layer, texture* t)
{
  switch (layer) {
    case PTG_LAYER_ROCK:
      rock_height_ = t;
      return;
    case PTG_LAYER_SOIL:
      soil_height_ = t;
      return;
  }

  device_->error("Failed to set the texture of a layer because the layer is not recognized.");
}

} // namespace ptg
