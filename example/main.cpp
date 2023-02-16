#include <ptg.h>

#include <stdlib.h>

#include <math.h>

#include "stb_image_write.h"

int
main()
{
  PtgDevice* device = PtgDevice_New(NULL);

  PtgModel* model = PtgModel_New(device);

  PtgModel_BeginPath(model);

  for (int i = 0; i < 16; i++)
    PtgModel_PlotPath(model, i, 8 * std::sin(6.28 * i / 16.0f));

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
