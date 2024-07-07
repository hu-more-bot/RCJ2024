#pragma once

class Serial {
public:
  Serial(const char *port);
  ~Serial();

  void send(void *data, unsigned long len);
  void send(const char *message);

  unsigned long receive(const char *buffer, unsigned long size);

private:
  int fd;
};
