#pragma once

// Logger
namespace Log {

// Cut-off Length of Tag
const int maxTagLen = 16;

// Log Debug Information, only visible for DEBUG Builds
void debug(const char *tag, const char *message, ...);

// Log Verbose Information
void verbose(const char *tag, const char *message, ...);

// Log Warning Messages
void warning(const char *tag, const char *message, ...);

// Log Error Message, usually "unrecoverable"
void error(const char *tag, const char *message, ...);

} // namespace Log