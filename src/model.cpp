#include "model.hpp"

namespace ptg {

model::model(std::shared_ptr<device> dev)
  : device_(std::move(dev))
{
}

memento
model::copy_current_memento() const
{
  return *current();
}

void
model::set_brush_size(const float brush_size)
{
  edit()->brush_size = brush_size;
}

void
model::begin_path()
{
  if (active_path_.has_value()) {
    // TODO : log error
    return;
  }

  active_path_ = path{ current()->brush_size, {}, current()->active_layer };
}

void
model::end_path()
{
  if (!active_path_.has_value()) {
    // TODO : log error
    return;
  }

  auto* m = edit();

  m->paths.emplace_back(std::move(active_path_.value()));

  m->operations.emplace_back(operation{ operation_kind::apply_path, static_cast<uint32_t>(m->paths.size() - 1) });
}

memento*
model::edit()
{
  mementos_.resize(memento_index_ + 1);

  memento copy{ mementos_.back() };

  mementos_.emplace_back(std::move(copy));

  memento_index_++;

  return &mementos_.back();
}

void
model::plot_path(const float x, const float y)
{
  if (!active_path_.has_value()) {
    // TODO : log error
    return;
  }

  active_path_->xy_coordinates.emplace_back(x);
  active_path_->xy_coordinates.emplace_back(y);
}

void
model::redo()
{
  if ((memento_index_ + 1) < mementos_.size())
    memento_index_++;
}

void
model::undo()
{
  if (memento_index_ > 0)
    memento_index_--;
}

} // namespace ptg
