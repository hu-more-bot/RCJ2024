#pragma once

#include <cstdarg>

namespace Log {

void custom(const char *color, const char *text, va_list args);

void debug(const char *text, ...);
void warning(const char *text, ...);
void error(const char *text, ...);
void assert(const char *text, ...);

} // namespace Log