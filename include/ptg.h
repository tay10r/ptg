#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * @defgroup ptg_device Device API
   *
   * @brief The API for dealing the compute engine.
   * */

  /**
   * @brief The type definition for a device object.
   *
   * @ingroup ptg_device
   * */
  typedef struct ptg_device PtgDevice;

  /**
   * @brief Creates a new device instance.
   *
   * @param gl_symbol_loader Used for loading symbols from an OpenGL context.
   *                         This function may be null, in which case everything is done in software.
   *
   * @return A new device instance.
   *
   * @ingroup ptg_device
   * */
  PtgDevice* PtgDevice_New(void* (*gl_symbol_loader)(const char* symbol_name));

  /**
   * @brief Releases memory allocated by a device.
   *
   * @ingroup ptg_device
   * */
  void PtgDevice_Delete(PtgDevice* device);

  /**
   * @defgroup ptg_model Model API
   *
   * @brief The API for dealing with terrain models.
   *
   * @details The model of a terrain can be thought of as the instructions to build the terrain.
   *          It does not contain any height fields or related data.
   * */

  /**
   * @brief A type definition for a terrain model.
   *
   * @ingroup ptg_model
   * */
  typedef struct ptg_model PtgModel;

  /**
   * @brief Creates a new terrain model.
   *
   * @param device The device to allocate the model data with.
   *
   * @return A new terrain model.
   *
   * @ingroup ptg_model
   * */
  PtgModel* PtgModel_New(PtgDevice* device);

  /**
   * @brief Releases memory allocated by a model.
   *
   * @ingroup ptg_model
   * */
  void PtgModel_Delete(PtgModel* model);

  void PtgModel_SetBrushSize(PtgModel* model, float brush_size);

  void PtgModel_BeginPath(PtgModel* model);

  void PtgModel_PlotPath(PtgModel* model, float x, float y);

  void PtgModel_EndPath(PtgModel* model);

  /**
   * @defgroup ptg_output Output API
   *
   * @brief The API for getting a usable terrain built from the library.
   */

  /**
   * @brief A type definition for the function used to write an image to the file system.
   *
   * @param filename The name to assign the file.
   *
   * @param w The width of the image, in pixels.
   *
   * @param h The height of the image, in pixels.
   *
   * @param comp The number of components in the image.
   *
   * @param data The pixel data associated with the image.
   *
   * @param pitch The number of bytes between rows in the image data.
   *
   * @return Zero on failure, non-zero on success.
   */
  typedef int (*ptg_write_png)(const char* filename, int w, int h, int comp, const void* data, int pitch);

  /**
   * @brief The type for a terrain output.
   *
   * @ingroup ptg_output
   */
  typedef struct ptg_output PtgOutput;

  /**
   * @brief Used to get usable terrain data.
   *
   * @param device The device to create the terrain output with.
   *
   * @param terrain_size The size of the output terrain.
   *
   * @return A new instance of a terrain output data.
   */
  PtgOutput* PtgOutput_New(PtgDevice* device, uint32_t terrain_size);

  /**
   * @brief Releases memory allocated by an output object.
   *
   * @param output The output object to release the memory of.
   *
   * @ingroup ptg_output
   * */
  void PtgOutput_Delete(PtgOutput* output);

  /**
   * @brief Prepares to bake the model into a usable terrain output.
   *
   * @param output The output object to bake the terrain into.
   *
   * @param model The model to bake.
   *
   * @returns The total number of steps required for the bake operation.
   *
   * @ingroup ptg_output
   */
  uint32_t PtgOutput_PrepareBake(PtgOutput* output, PtgModel* model);

  /**
   * @brief Iterates the bake job for an output object.
   *
   * @param output The output object being baked.
   *
   * @return True on success, false if there are no more bake steps to take.
   *         In other words, if this function returns false then the baking is complete.
   *
   * @ingroup ptg_output
   */
  bool PtgOutput_IterateBake(PtgOutput* output);

  /**
   * @brief Saves the total height of the terrain as a PNG file.
   *
   * @param output The output terrain to save the height map of.
   *
   * @param path The path to save the file at.
   *
   * @param png_write_function The function used to write PNG files.
   *                           The function signature is compatible with the STB library.
   *
   * @return True if the operation was successful, false if an error occurred.
   */
  bool PtgOutput_SaveHeightPng(PtgOutput* output, const char* path, ptg_write_png png_write_function);

#ifdef __cplusplus
} /* extern "C" */
#endif
