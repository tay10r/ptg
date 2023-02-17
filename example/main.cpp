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

  PtgModel_SetBrushSize(model, 25.0f);

  PtgModel_BeginPath(model);

  const int steps = 128;

  for (int i = 0; i < steps; i++)
    PtgModel_PlotPath(model, i * 1000.0f / steps, 500.0f + 500.0f * sin(6.28f * static_cast<float>(i) / steps));

  PtgModel_EndPath(model);

  PtgOutput* output = PtgOutput_New(device, 256);

  const auto step_count = PtgOutput_PrepareBake(output, model);

  for (uint32_t i = 0; i < step_count; i++)
    PtgOutput_IterateBake(output);

  PtgOutput_SaveHeightPng(output, "result.png", stbi_write_png);

  PtgOutput_Delete(output);

  PtgModel_Delete(model);

  PtgDevice_Delete(device);

  return EXIT_SUCCESS;
}
