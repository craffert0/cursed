#pragma once

#include <sys/types.h>

class Writer {
public:
  virtual ~Writer() = default;

  virtual ssize_t write(const void* buffer, size_t nbyte) = 0;
};
