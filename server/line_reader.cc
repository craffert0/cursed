#include "line_reader.h"

#include "exception.h"

#include <cstring>

using namespace std;

LineReader::LineReader(Reader& reader)
    : reader_(reader), curr_line_(scratch_, 0), rest_(scratch_, 0) {}

string_view LineReader::next() {
  // Shift previous result out of `rest_` and create `buffer` view of all the
  // bytes.
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
