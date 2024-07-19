/**
 * @brief C Logging Library for Artifex
 *
 * @date 2024.06.xx TODO
 * @author SamTheDev
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Log Debug Information, only visible for DEBUG Builds
void ax_debug(const char* tag, const char* message, ...);

// Log Verbose Information
void ax_verbose(const char* tag, const char* message, ...);

// Log Warning Messages
void ax_warning(const char* tag, const char* message, ...);

// Log Error Message, usually "unrecoverable"
void ax_error(const char* tag, const char* message, ...);

// Assert
void ax_assert(int condition, const char* tag, const char* message, ...);

#ifdef __cplusplus
}
#endif