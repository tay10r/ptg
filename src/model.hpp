#pragma once

#include <optional>
#include <vector>

#include <stdint.h>

namespace ptg {

struct path final
{
  float brush_size{ 4.0f };

  std::vector<float> xy_coordinates;
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
};

class model final
{
public:
  void begin_path();

  void end_path();

  void plot_path(float x, float y);

  void undo();

  void redo();

  /// @brief Makes a copy of the current memento.
  ///        Used when starting a bake operation.
  ///
  /// @return A copy of the current memento.
  memento copy_current_memento() const;

private:
  const memento* current() const { return &mementos_.at(memento_index_); }

  memento* edit();

private:
  uint32_t memento_index_{ 0 };

  std::vector<memento> mementos_{ memento{} };

  std::optional<path> active_path_;
};

} // namespace ptg
