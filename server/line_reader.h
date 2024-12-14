#pragma once

#include "reader.h"

#include <string_view>

/** Converts unconstrained reads into line-by-line reads, stripping CRLF. */
class LineReader {
public:
  explicit LineReader(Reader& reader);

  /** Return next line. Only valid until the next `next()`. */
  std::string_view next();

private:
  Reader& reader_;
  char scratch_[256];          // all the bytes
  std::string_view curr_line_; // view of most recent `next()` result
  std::string_view rest_;      // view of all the remaining bytes
};
