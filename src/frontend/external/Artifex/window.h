/**
 * @brief C Windowing Library for Artifex
 *
 * @date 2024.06.xx TODO
 * @author SamTheDev
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "keys.h"

// Artifex Window Type
typedef struct axWindow_* axWindow;

enum axWindowKey;

// Check if Window is valid
int axWindowIsValid(axWindow window);

// Create Window
int axWindowCreate(axWindow* window, const char* title, int width, int height);

// Destroy Window
void axWindowDestroy(axWindow* window);

// Update Window
int axWindowUpdate(axWindow window);

// Exit Window
void axWindowExit(axWindow window);

// Get Window Position
void axWindowPos(axWindow window, int* x, int* y);

// Get Window Size
void axWindowSize(axWindow window, int* width, int* height);

// Get Window Ratio
float axWindowRatio(axWindow window);

// Make Window Fullscreen
void axWindowSetFullscreen(axWindow window, int en, int hideCursor);

// Get if Window is Fullscreened or not
int axWindowIsFullscreen(axWindow window);

// Set Window VSYNC
void axWindowSetVSync(axWindow window, int interval);

// ---- User Input

// Get Cursor Position
int axWindowGetCursor(axWindow window, float* x, float* y);

// Get Scroll Position
// int axWindowGetScroll(axWindow window, float* x, float* y);

// Get Key State
int axWindowGetKeyState(axWindow window, enum axWindowKey key);

// Get Process Address callback
typedef void (*axWindowProcAddr)();
axWindowProcAddr axWindowGetAddress(const char* procname);

// Make Window Context Current
void axWindowMakeCurrent(axWindow window);

#ifdef __cplusplus
}
#endif