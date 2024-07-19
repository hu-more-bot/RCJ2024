/**
 * @brief C Clock Library for Artifex
 *
 * @date 2024.06.xx TODO
 * @author SamTheDev
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Clock
typedef struct {
    // TODO
} axClock;

// Get UNIX Time
unsigned long axClockUNIX();

// Get Date String ("YYYY.MM.DD")
int axClockDate(char str[11]);

// Get UTC Time String ("hh.mm.ss")
unsigned long axClockTime(char str[9]);

// Get Current Time
float axClockNow();

#ifdef __cplusplus
}
#endif