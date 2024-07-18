#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Simple Linux Serial Device
typedef struct Serial *serial_t;

typedef void (*serialCallback)(unsigned char *, unsigned long, void *);

// Check if serial is healthy
int serialIsOK(serial_t serial);

// Create Serial
int serialCreate(serial_t *serial, const char *port);

// Destroy Serial
void serialDestroy(serial_t *serial);

// Send Message
int serialSend(serial_t serial, void *data, unsigned long len);

// TODO receive message
// unnecessary for this project

#ifdef __cplusplus
}
#endif