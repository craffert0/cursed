#pragma once

#include "line_reader.h"

#include <map>
#include <string>

struct HttpRawRequest {
  std::string method;
  std::string path;
  std::map<std::string, std::string> path_args;
  std::string version;
  std::map<std::string, std::string> headers;
  std::string remainder;
};

HttpRawRequest slurp_raw_request(LineReader& reader);
