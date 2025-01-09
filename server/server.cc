#include "server.h"

#include "exception.h"
#include "http_raw_request.h"
#include "socket.h"

#include <iostream>
#include <sstream>

using namespace std;

void Server::serve(Socket& sock) {
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

void Server::handle_get(Writer& writer, const HttpRawRequest& request) const {
  // "/get?key=KEY"
  ASSERT(request.path == "/get", 404, "not found");
  ASSERT((request.path_args.size() == 1 &&
          request.path_args.begin()->first == "key"),
         400, "bad request");
  const auto it = db_.find(request.path_args.begin()->second);
  ASSERT(it != db_.end(), 404, "not found");
  reply(writer, 200, it->second);
}

void Server::handle_post(Writer& writer, const HttpRawRequest& request) {
  // "/set?KEY=VALUE"
  ASSERT(request.path == "/set", 405, "not allowed");
  ASSERT(request.path_args.size() == 1, 400, "bad request");
  const auto it = request.path_args.begin();
  db_.emplace(it->first, it->second);
  reply(writer, 200, "ok\n");
}

void Server::reply(Writer& writer, int code, string_view body) const {
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
