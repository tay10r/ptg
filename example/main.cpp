#include <ptg.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "stb_image_write.h"

static void
log_callback(void* callback_data, const PtgSeverity severity, const char* msg)
{
  (void)callback_data;

  switch (severity) {
    case PTG_INFO:
      printf("INFO: ");
      break;
    case PTG_WARN:
      printf("WARN: ");
      break;
    case PTG_ERROR:
      printf("ERROR: ");
      break;
  }

  printf("%s\n", msg);
}

int
main()
{
  PtgDevice* device = PtgDevice_New(NULL, NULL, log_callback);

  PtgModel* model = PtgModel_New(device);

  //=========================//
  // Apply sinusoidal brush. //
  //=========================//

  PtgModel_SetBrushSize(model, 25.0f);

  PtgModel_BeginPath(model);

  const int steps = 1;

  for (int i = 0; i < steps; i++)
    PtgModel_PlotPath(model, i * 1000.0f / steps, 500.0f + 500.0f * sin(6.28f * static_cast<float>(i) / steps));

  PtgModel_EndPath(model);

  //========================//
  // Compute Output Terrain //
  //========================//

  PtgOutput* output = PtgOutput_New(device, 16);

  const auto step_count = PtgOutput_PrepareBake(output, model);

  for (uint32_t i = 0; i < step_count; i++)
    PtgOutput_IterateBake(output);

  PtgOutput_SaveHeightPng(output, "result.png", stbi_write_png);

  //================//
  // Render Terrain //
  //================//

  PtgRender* render = PtgRender_New(device, 1024);

  PtgRender_SetCameraPosition(render, 0, 10, 0);

  PtgRender_SetCameraRotation(render, 90, 0, 0);

  PtgRender_SetCameraPerspective(render, 1.0f, 45.0f, 0.01f, 2000.0f);

  for (int i = 0; i < 128; i++)
    PtgRender_Iterate(render);

  PtgRender_SavePng(render, "render.png", stbi_write_png);

  PtgRender_Delete(render);

  //=========//
  // Cleanup //
  //=========//

  PtgOutput_Delete(output);

  PtgModel_Delete(model);

  PtgDevice_Delete(device);

  return EXIT_SUCCESS;
}
