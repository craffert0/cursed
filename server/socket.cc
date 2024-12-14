#include "socket.h"

#include <unistd.h>

Socket::~Socket() { close(sock_); }

ssize_t Socket::read(void* buffer, size_t nbyte) {
  return ::read(sock_, buffer, nbyte);
}

ssize_t Socket::write(const void* buffer, size_t nbyte) {
  return ::write(sock_, buffer, nbyte);
}
