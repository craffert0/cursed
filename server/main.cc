/**
 * Before your interview, write a program that runs a server that is
 * accessible on http://localhost:4000/. When your server receives a
 * request on http://localhost:4000/set?somekey=somevalue it should store
 * the passed key and value in memory. When it receives a request on
 * http://localhost:4000/get?key=somekey it should return the value stored
 * at somekey.
 *
 * curl --output - -X POST http://localhost:4000/set?somekey=somevalue
 * curl --output - http://localhost:4000/get?key=somekey
 */

#include "server.h"
#include "socket.h"

#include <cerrno>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>

using namespace std;

namespace {

/** Return a TCP socket listening on PORT, or -errno on error. */
int make_socket(int port) {
  const int sock = ::socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    return -errno;
  }

  const sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_addr.s_addr = INADDR_ANY,
      .sin_port = htons(port),
  };
  if (::bind(sock, (const sockaddr*)&addr, sizeof(addr)) != 0) {
    return -errno;
  }

  if (::listen(sock, 128) != 0) {
    return -errno;
  }

  return sock;
}

} // namespace

int main(int argc, const char** argv) {
  if (argc != 2) {
    cerr << "Usage: server PORT" << endl;
    return 1;
  }

  const int listener = make_socket(atoi(argv[1]));
  if (listener < 0) {
    cerr << "oops: " << strerror(-listener) << endl;
    return -listener;
  }

  Server server;
  while (true) {
    Socket sock(::accept(listener, nullptr, nullptr));
    server.serve(sock);
  }
  return 0;
}
