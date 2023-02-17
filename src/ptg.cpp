#include <ptg.h>

#include "cpu_device.hpp"
#include "model.hpp"
#include "output.hpp"
#include "render.hpp"

//============//
// Device API //
//============//

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

//===========//
// Model API //
//===========//

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

//============//
// Output API //
//============//

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

//============//
// Render API //
//============//

struct ptg_render
{
  ptg::render impl;
};

PtgRender*
PtgRender_New(PtgDevice* device, uint32_t image_size)
{
  return new ptg_render{ ptg::render(device->impl, image_size) };
}

void
PtgRender_Delete(PtgRender* render)
{
  delete render;
}

void
PtgRender_SetCameraPosition(PtgRender* render, const float x, const float y, const float z)
{
  render->impl.get_camera()->position = glm::vec3(x, y, z);
}

void
PtgRender_SetCameraRotation(PtgRender* render, const float x, const float y, const float z)
{
  render->impl.get_camera()->rotation = glm::vec3(x, y, z);
}

void
PtgRender_SetCameraPerspective(PtgRender* render,
                               const float aspect,
                               const float fovy,
                               const float near,
                               const float far)
{
  auto* cam = render->impl.get_camera();
  cam->aspect = aspect;
  cam->fov = fovy;
  cam->near = near;
  cam->far = far;
}

void
PtgRender_Iterate(PtgRender* render)
{
  render->impl.iterate();
}

void
PtgRender_SavePng(PtgRender* render, const char* path, ptg_write_png png_writer)
{
  render->impl.save_to_png(path, png_writer);
}
