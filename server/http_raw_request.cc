#include "http_raw_request.h"

#include "exception.h"
#include "line_reader.h"

#include <iostream>

using namespace std;

namespace {

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

} // namespace

HttpRawRequest slurp_raw_request(LineReader& reader) {
  HttpRawRequest request;

  auto line = reader.next();
  split_request(&request, line);
  while (!(line = reader.next()).empty()) {
    split_header(&request, line);
  }

  return request;
}
