#pragma once

#include <sys/types.h>

class Reader {
public:
  virtual ~Reader() = default;

  /** Return number of bytes read, 0 on EOF, -1 on error. */
  virtual ssize_t read(void* buffer, size_t nbyte) = 0;
};
