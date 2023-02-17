#pragma once

#include "device.hpp"

#include <optional>
#include <vector>
#include <memory>

#include <stdint.h>

#include "ptg.h"

namespace ptg {

struct path final
{
  /// The size of the brush applying the path.
  float brush_size{ 16.0f };

  /// The coordinates of each point in the path.
  std::vector<float> xy_coordinates;

  /// The layer on which to apply this path.
  PtgLayer layer;
};

enum class operation_kind
{
  apply_path
};

struct operation final
{
  operation_kind kind;

  uint32_t index;
};

/// @brief Represents a completely self-contained copy of the terrain model.
///        Can be thought of as a snapshot of the terrain model.
struct memento final
{
  /// @brief The size of the brush to be used in the next path.
  float brush_size{ 4.0f };

  /// @brief The number of meters per axis.
  float meters_per_axis{ 1000.0f };

  /// @brief The paths to be applied to the memento.
  std::vector<path> paths;

  /// The operations, in order, to apply to the terrain when baking.
  std::vector<operation> operations;

  /// The currently selected layer.
  PtgLayer active_layer{ PTG_LAYER_ROCK };
};

class model final
{
public:
  /// @brief Constructs a new model instance.
  ///
  /// @parma dev The device responsible for creating the model.
  explicit model(std::shared_ptr<device> dev);

  void set_brush_size(float brush_size);

  void begin_path();

  void end_path();

  void plot_path(float x, float y);

  void undo();

  void redo();

  /// @brief Makes a copy of the current memento.
  ///        Used when starting a bake operation.
  ///
  /// @return A copy of the current memento.
  [[nodiscard]] memento copy_current_memento() const;

private:
  [[nodiscard]] const memento* current() const { return &mementos_.at(memento_index_); }

  memento* edit();

private:
  /// The device that constructed this model.
  std::shared_ptr<device> device_;

  uint32_t memento_index_{ 0 };

  std::vector<memento> mementos_{ memento{} };

  std::optional<path> active_path_;
};

} // namespace ptg
