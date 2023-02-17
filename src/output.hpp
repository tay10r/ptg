#pragma once

#include "device.hpp"
#include "model.hpp"
#include "texture.hpp"

#include <ptg.h>

#include <memory>
#include <optional>

namespace ptg {

class output final
{
public:
  explicit output(std::shared_ptr<device> dev, const uint32_t terrain_size);

  output(const output&) = delete;

  output(output&&) = delete;

  output& operator=(const output&) = delete;

  output& operator=(output&&) = delete;

  ~output();

  /// @brief Gets the size of the terrain, in both axes.
  ///
  /// @return The size of the terrain, in both axes.
  uint32_t get_terrain_size() const { return terrain_size_; }

  /// @brief Saves the total height of each cell in a PNG file.
  /// @param path The path to save the PNG file to.
  /// @param png_writer Used to serialize the PNG data.
  /// @return True on success, false on failure.
  bool save_height_png(const char* path, ptg_write_png png_writer);

  /// @brief Prepares to bake a model.
  ///
  /// @param m The model to prepare.
  ///
  /// @return The number of steps required to bake the output.
  uint32_t prepare_bake(const model& m);

  /// @brief Iterates the bake operation.
  ///
  /// @return True on success, false if the bake is done.
  bool iterate_bake();

private:
  void apply_raise_operation(const path& p);

  /// @brief Gets a texture associated with a specific layer.
  ///
  /// @param layer The layer to get the texture of.
  ///
  /// @return The texture of the specified layer.
  texture* get_layer_texture(PtgLayer layer);

  /// @brief Sets the texture associated with a specific layer.
  ///
  /// @param layer The layer to set the texture of.
  ///
  /// @param t The texture to assign the layer.
  void set_layer_texture(PtgLayer layer, texture* t);

  /// @brief Used to store data related to a bake job.
  struct bake_job final
  {
    /// @brief The memento to bake.
    memento m;

    /// @brief The index of the operation to execute next.
    uint32_t operation_index{ 0 };
  };

  /// The device that owns the output object.
  std::shared_ptr<device> device_;

  /// The size of the terrain in each axis.
  uint32_t terrain_size_{ 0 };

  /// The rock layer height texture.
  texture* rock_height_;

  /// The soil layer height texture.
  texture* soil_height_;

  std::optional<bake_job> bake_job_;
};

} // namespace ptg
