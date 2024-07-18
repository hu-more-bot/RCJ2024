#include <serial.h>

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <Artifex/log.h>

#define TAG "serial"

int set_interface_attribs(int fd, int speed, int parity) {
  struct termios tty;
  if (tcgetattr(fd, &tty) != 0) {
    ax_error(TAG, "error from tcgetattr");
    return -1;
  }

  cfsetospeed(&tty, speed);
  cfsetispeed(&tty, speed);

  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
  // disable IGNBRK for mismatched speed tests; otherwise receive break
  // as \000 chars
  tty.c_iflag &= ~IGNBRK; // disable break processing
  tty.c_lflag = 0;        // no signaling chars, no echo,
                          // no canonical processing
  tty.c_oflag = 0;        // no remapping, no delays
  tty.c_cc[VMIN] = 0;     // read doesn't block
  tty.c_cc[VTIME] = 5;    // 0.5 seconds read timeout

  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

  tty.c_cflag |= (CLOCAL | CREAD);   // ignore modem controls,
                                     // enable reading
  tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
  tty.c_cflag |= parity;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;

  if (tcsetattr(fd, TCSANOW, &tty) != 0) {
    ax_error(TAG, "error from tcsetattr");
    return -1;
  }
  return 0;
}

void set_blocking(int fd, int should_block) {
  struct termios tty;
  memset(&tty, 0, sizeof tty);
  if (tcgetattr(fd, &tty) != 0) {
    ax_error(TAG, "error from tggetattr");
    return;
  }

  tty.c_cc[VMIN] = should_block ? 1 : 0;
  tty.c_cc[VTIME] = 5; // 0.5 seconds read timeout

  if (tcsetattr(fd, TCSANOW, &tty) != 0)
    ax_error(TAG, "error setting term attributes");
}

struct Serial {
  int fd;
};

int serialIsOK(serial_t serial) { return serial && serial->fd; }

int serialCreate(serial_t *serial, const char *port) {
  if (!serial)
    return 1;

  serial_t ser = *serial = malloc(sizeof(struct Serial));
  if (!ser) {
    ax_error(TAG, "failed to allocate memory");
    return 1;
  }

  ser->fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
  if (ser->fd < 0) {
    ax_error(TAG, "error opening port");
    return 1;
  }

  set_interface_attribs(ser->fd, B115200,
                        0); // set speed to 115,200 bps, 8n1 (no parity)
  set_blocking(ser->fd, 0); // set no blocking

  ax_verbose(TAG, "initialized");
  return 0;
}

void serialDestroy(serial_t *serial) {
  if (!serial || !*serial)
    return;

  close((*serial)->fd);

  free(*serial);
  *serial = NULL;

  ax_verbose(TAG, "destroyed");
}

int serialSend(serial_t serial, void *data, unsigned long len) {
  if (!serialIsOK(serial))
    return 1;

  write(serial->fd, data, len);
  return 0;
}

// unsigned long Serial::receive(const char *buffer, unsigned long size) {
// return read(fd, (void *)buffer, size);
// }