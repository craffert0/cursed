#pragma once

#include "reader.h"
#include "writer.h"

class Socket : public Reader, public Writer {
public:
  explicit Socket(int sock) : sock_(sock) {}
  ~Socket() override;

  ssize_t read(void* buffer, size_t nbyte) override;
  ssize_t write(const void* buffer, size_t nbyte) override;

private:
  const int sock_;
};
