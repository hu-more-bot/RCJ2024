/**
* @brief C Color Library for Artifex
*
* @date 2024.06.xx TODO
* @author SamTheDev
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Color Component Type
#define AX_COLOR_TYPE float

// RGBA Color
typedef struct {
    AX_COLOR_TYPE r, g, b, a;
} axColor;

#ifdef __cplusplus
}
#endif