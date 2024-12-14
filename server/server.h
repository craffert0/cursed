#pragma once

#include <map>
#include <string>
#include <string_view>

struct HttpRawRequest;
class Reader;
class Socket;
class Writer;

class Server {
public:
  void serve(Socket& sock);

private:
  void handle_get(Writer& writer, const HttpRawRequest& request) const;
  void handle_post(Writer& writer, const HttpRawRequest& request);
  void reply(Writer& writer, int code, std::string_view body) const;

private:
  std::map<std::string, std::string> db_;
};
