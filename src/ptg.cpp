#include <ptg.h>

#include "cpu_device.hpp"
#include "model.hpp"
#include "output.hpp"

struct ptg_device
{
  std::shared_ptr<ptg::device> impl;
};

PtgDevice*
PtgDevice_New(const ptg_gl_symbol_loader gl_symbol_loader, void* logger_data, ptg_log_callback logger_func)
{
  auto* device = new ptg_device;

  if (!gl_symbol_loader)
    device->impl = ptg::create_cpu_device(logger_data, logger_func);

  return device;
}

void
PtgDevice_Delete(PtgDevice* device)
{
  delete device;
}

struct ptg_model
{
  ptg::model impl;
};

PtgModel*
PtgModel_New(PtgDevice* device)
{
  return new ptg_model{ ptg::model(device->impl) };
}

void
PtgModel_Delete(PtgModel* model)
{
  delete model;
}

void
PtgModel_SetBrushSize(PtgModel* model, float brush_size)
{
  model->impl.set_brush_size(brush_size);
}

void
PtgModel_BeginPath(PtgModel* model)
{
  model->impl.begin_path();
}

void
PtgModel_EndPath(PtgModel* model)
{
  model->impl.end_path();
}

void
PtgModel_PlotPath(PtgModel* model, float x, float y)
{
  model->impl.plot_path(x, y);
}

struct ptg_output
{
  ptg::output impl;

  ptg_output(PtgDevice* device, const uint32_t terrain_size)
    : impl(device->impl, terrain_size)
  {
  }
};

PtgOutput*
PtgOutput_New(PtgDevice* device, const uint32_t terrain_size)
{
  return new ptg_output(device, terrain_size);
}

void
PtgOutput_Delete(PtgOutput* output)
{
  delete output;
}

uint32_t
PtgOutput_PrepareBake(PtgOutput* output, PtgModel* model)
{
  return output->impl.prepare_bake(model->impl);
}

bool
PtgOutput_IterateBake(PtgOutput* output)
{
  return output->impl.iterate_bake();
}

bool
PtgOutput_SaveHeightPng(PtgOutput* output, const char* filename, ptg_write_png png_writer)
{
  return output->impl.save_height_png(filename, png_writer);
}
