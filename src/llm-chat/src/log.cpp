#include <cstdlib>
#include <log.hpp>

#include <cstdio>

const char *c_clear = "\033[39m\033[49m"; // default
const char *c_debug = "\033[94m";         // blue text
const char *c_warning = "\033[1;93m";     // yellow text
const char *c_error = "\033[91m";         // red text
const char *c_assert = "\033[41m";        // red background

namespace Log {

void custom(const char *color, const char *text, va_list args) {
  printf("%s", color);
  vprintf(text, args);
  printf("%s\n", c_clear);
}

void debug(const char *text, ...) {
  va_list va;
  va_start(va, text);

  custom(c_debug, text, va);

  va_end(va);
}

void warning(const char *text, ...) {
  va_list va;
  va_start(va, text);

  custom(c_warning, text, va);

  va_end(va);
}

void error(const char *text, ...) {
  va_list va;
  va_start(va, text);

  custom(c_error, text, va);

  va_end(va);
}

void assert(const char *text, ...) {
  va_list va;
  va_start(va, text);

  custom(c_assert, text, va);

  va_end(va);

  exit(1);
}

} // namespace Log