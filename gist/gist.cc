// This is a consolidation of the code in: ..//server

#include <cerrno>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>

using namespace std;

/****************************************************************************/
/*                                 exception.h                              */
/****************************************************************************/

class Exception : public domain_error {
public:
  Exception(int code, const char* what) : domain_error(what), code_(code) {}

  int code() const { return code_; }

private:
  const int code_;
};

/** If `condition` is false, throw Exception with error `code`. */
#define ASSERT(condition, code, what)                                        \
  if (condition)                                                             \
    ;                                                                        \
  else                                                                       \
    throw Exception(code, what)

/****************************************************************************/
/*                   socket.h/cc consolidates reader & writer               */
/****************************************************************************/

class Socket {
public:
  explicit Socket(int sock) : sock_(sock) {}

  ssize_t read(void* buffer, size_t nbyte) {
    return ::read(sock_, buffer, nbyte);
  }

  ssize_t write(const void* buffer, size_t nbyte) {
    return ::write(sock_, buffer, nbyte);
  }

private:
  const int sock_;
};

/****************************************************************************/
/*                                linereader.h/cc                           */
/****************************************************************************/

class LineReader {
public:
  explicit LineReader(Socket& reader)
      : reader_(reader), curr_line_(scratch_, 0), rest_(scratch_, 0) {}

  /** Return next line. Only valid until the next `next()`. */
  string_view next() {
    // Shift previous result out of `rest_` and create `buffer` view of all
    // the bytes.
    memmove(scratch_, rest_.data(), rest_.size());
    string_view buffer(scratch_, rest_.size());

    // Find next CRLF, reading off the socket until we find it.
    size_t crlf;
    while (((crlf = buffer.find("\r\n")) == string_view::npos) &&
           (buffer.size() != sizeof(scratch_))) {
      const auto rc = reader_.read(scratch_ + buffer.size(),
                                   sizeof(scratch_) - buffer.size());
      ASSERT(rc >= 0, 500, "Socket Error");
      ASSERT(rc != 0, 499, "Client Closed Request");
      buffer = {scratch_, buffer.size() + rc};
    }
    ASSERT(crlf != string_view::npos, 431, "Request Header Fields Too Long");

    // Split buffer on CRLF.
    curr_line_ = buffer.substr(0, crlf);
    rest_ = buffer.substr(crlf + 2);
    return curr_line_;
  }

private:
  Socket& reader_;
  char scratch_[256];     // all the bytes
  string_view curr_line_; // view of most recent `next()` result
  string_view rest_;      // view of all the remaining bytes
};

/****************************************************************************/
/*                            http_raw_request.h/cc                         */
/****************************************************************************/

struct HttpRawRequest {
  string method;
  string path;
  map<string, string> path_args;
  string version;
  map<string, string> headers;
  string remainder;
};

string_view trim(string_view token) {
  token = token.substr(token.find_first_not_of(' '));
  return token.substr(0, token.find_last_not_of(' ') + 1);
}

void split_path(HttpRawRequest* request, string_view fullpath) {
  // "/set?somekey=somevalue"
  // "/foo/bar/baz"
  const auto question = fullpath.find('?');
  if (question == string_view::npos) {
    // "/foo/bar/baz"
    request->path = fullpath;
  } else {
    // "/set?somekey=somevalue"
    // "/set?somekey=somevalue&key2=value2&key3=value3"
    request->path = fullpath.substr(0, question);
    auto args = fullpath.substr(question + 1);
    while (!args.empty()) {
      const auto amp = args.find('&');
      const auto arg =
          (amp == string_view::npos) ? args : args.substr(0, amp);
      args =
          (amp == string_view::npos) ? string_view() : args.substr(amp + 1);
      const auto eq = arg.find('=');
      ASSERT(eq != string_view::npos, 400, "invalid format");
      request->path_args.emplace(trim(arg.substr(0, eq)),
                                 trim(arg.substr(eq + 1)));
    }
  }
}

void split_request(HttpRawRequest* request, string_view line) {
  // "PUT /set?somekey=somevalue HTTP/1.1"
  size_t sz = line.find(' ');
  ASSERT(sz != string_view::npos, 400, "invalid format");
  request->method = trim(line.substr(0, sz));

  // "/set?somekey=somevalue HTTP/1.1"
  line = trim(line.substr(sz + 1));
  sz = line.find(' ');
  ASSERT(sz != string_view::npos, 400, "invalid format");
  split_path(request, trim(line.substr(0, sz)));
  request->version = trim(line.substr(sz + 1));
}

void split_header(HttpRawRequest* request, string_view line) {
  // "User-Agent: Mozilla/5.0
  const auto colon = line.find(':');
  ASSERT(colon != string_view::npos, 400, "invalid header line");
  request->headers.emplace(trim(line.substr(0, colon)),
                           trim(line.substr(colon + 1)));
}

HttpRawRequest slurp_raw_request(LineReader& reader) {
  HttpRawRequest request;

  auto line = reader.next();
  split_request(&request, line);
  while (!(line = reader.next()).empty()) {
    split_header(&request, line);
  }

  return request;
}

/****************************************************************************/
/*                                 server.h/cc                              */
/****************************************************************************/

class Server {
public:
  void serve(Socket& sock) {
    try {
      LineReader reader(sock);
      const auto request = slurp_raw_request(reader);
      if (request.method == "GET") {
        handle_get(sock, request);
      } else {
        ASSERT(request.method == "POST", 400, "bad request");
        handle_post(sock, request);
      }
    } catch (const Exception& e) {
      cerr << e.code() << ": " << e.what() << endl;
      reply(sock, e.code(), e.what());
    } catch (const exception& e) {
      cerr << e.what() << endl;
      reply(sock, 500, e.what());
    } catch (...) {
      cerr << "unknown exception" << endl;
      reply(sock, 500, "unknown exception");
    }
  }

private:
  void handle_get(Socket& writer, const HttpRawRequest& request) const {
    // "/get?key=KEY"
    ASSERT(request.path == "/get", 404, "not found");
    ASSERT((request.path_args.size() == 1 &&
            request.path_args.begin()->first == "key"),
           400, "bad request");
    const auto it = db_.find(request.path_args.begin()->second);
    ASSERT(it != db_.end(), 404, "not found");
    reply(writer, 200, it->second);
  }

  void handle_post(Socket& writer, const HttpRawRequest& request) {
    // "/set?KEY=VALUE"
    ASSERT(request.path == "/set", 405, "not allowed");
    ASSERT(request.path_args.size() == 1, 400, "bad request");
    const auto it = request.path_args.begin();
    db_.emplace(it->first, it->second);
    reply(writer, 200, "ok\n");
  }

  void reply(Socket& writer, int code, string_view body) const {
    ostringstream ostr;
    ostr << "HTTP/1.1 " << code << "\r\n"
         << "server: cursed\r\n"
         << "content-type: text/plain\r\n"
         << "content-length: " << body.size() << "\r\n"
         << "connection: close\r\n"
         << "\r\n"
         << body;
    const auto str = ostr.str();
    writer.write(str.c_str(), str.size());
  }

private:
  map<string, string> db_;
};

/****************************************************************************/
/*                                   main.cc                                */
/****************************************************************************/

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
