#pragma once

#include <stdexcept>

class Exception : public std::domain_error {
public:
  Exception(int code, const char* what)
      : std::domain_error(what), code_(code) {}

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
