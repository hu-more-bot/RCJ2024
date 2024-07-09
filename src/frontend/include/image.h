#pragma once

#include <Artifex/log.h>
#include <Artifex/renderer.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <string.h>

// Load Texture from File
unsigned int loadTexture(axRenderer renderer, const char *path) {
  int w, h, ch;
  unsigned char *data = stbi_load(path, &w, &h, &ch, 4);

  if (!data) {
    ax_warning("loadTexture", "failed to open file");
    return 0; // failed to open file
  }

  unsigned int id = axRendererLoadTexture(renderer, w, h, ch, data);
  free(data);
  return id;
}

/**
 * @brief Create Cropped Image from original (RGB)
 *
 * @param data Original Image
 * @param width Original Image Width
 * @param height Original Image Height
 *
 * @param newData Output Image Pointer (should be allocated)
 * @param newWidth Output Image Width (equals to [width] if 0 or less)
 * @param newHeight Output Image Height (equals to [width] if 0 or less)
 *
 * @param centerX Center of Output Image (relative to original image in pixels)
 * @param centerY Center of Output Image (relative to original image in pixels)
 *
 * @param background Fill Color where the output image may not be in-frame
 */
void cropImage(unsigned char *data, int width, int height,
               unsigned char *newData, int newWidth, int newHeight, int centerX,
               int centerY) {
  if (!data || width <= 0 || height <= 0 || !newData) {
    ax_warning("cropImage", "no image data");
    return;
  }

  // left to right, top to bottom
  for (int y = 0; y < newHeight; y++) {
    for (int x = 0; x < newWidth; x++) {
      long originX = centerX - newWidth / 2 + x;
      long originY = centerY - newHeight / 2 + y;

      unsigned long index = (y * newWidth + x) * 3;
      unsigned long originIndex = (originY * width + originX) * 3;

      // Set Pixel
      if (originX < 0 || originY < 0 || originX >= width || originY >= height)
        memset(newData + index, 0, 3);
      else
        memcpy(newData + index, data + originIndex, 3);
    }
  }
}