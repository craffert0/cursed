#include "http_raw_request.h"

#include "exception.h"
#include "line_reader.h"
#include "reader.h"

#include "gtest/gtest.h"

#include <algorithm>
#include <string_view>

using namespace std;
using namespace testing;

/** A Reader that provides the results in user-defined chunks. */
class FakeReader : public Reader {
public:
  FakeReader(string_view buffer, size_t chunk_size)
      : buffer_(buffer), chunk_size_(chunk_size) {}

  ssize_t read(void* buffer, size_t nbyte) override {
    const auto chunk_size = min({nbyte, chunk_size_, buffer_.size()});
    memcpy(buffer, buffer_.data(), chunk_size);
    buffer_ = buffer_.substr(chunk_size);
    return chunk_size;
  }

private:
  string_view buffer_;
  const size_t chunk_size_;
};

class HttpRawRequestTest : public TestWithParam<size_t> {
protected:
  HttpRawRequest slurp(string_view buffer) {
    FakeReader fake(buffer, GetParam());
    LineReader reader(fake);
    const auto result = slurp_raw_request(reader);
    return result;
  }
};

// Run identical tests, but for various chunk sizes to simulate different
// quality connections.
INSTANTIATE_TEST_SUITE_P(HttpRawRequestTest, HttpRawRequestTest,
                         Values(1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 1024));

TEST_P(HttpRawRequestTest, basics) {
  static constexpr string_view kRequest =
      "GET /foo HTTP/1.1\r\n"
      "Host: localhost:4000 \r\n"
      "User-Agent: Mozilla/5.0\r\n"
      "Accept:  text/html,application/xhtml+xml,application/xml\r\n"
      "Priority: u=0, i\r\n"
      "Accept-Encoding:gzip, deflate\r\n"
      "Connection:    keep-alive\r\n"
      "\r\n";

  const auto result = slurp(kRequest);
  EXPECT_EQ("GET", result.method);
  EXPECT_EQ("/foo", result.path);
  EXPECT_TRUE(result.path_args.empty());
  EXPECT_EQ("HTTP/1.1", result.version);
  EXPECT_EQ(6, result.headers.size());
  EXPECT_EQ(result.headers.at("Host"), "localhost:4000");
  EXPECT_EQ(result.headers.at("User-Agent"), "Mozilla/5.0");
  EXPECT_EQ(result.headers.at("Accept"),
            "text/html,application/xhtml+xml,application/xml");
  EXPECT_EQ(result.headers.at("Priority"), "u=0, i");
  EXPECT_EQ(result.headers.at("Accept-Encoding"), "gzip, deflate");
  EXPECT_EQ(result.headers.at("Connection"), "keep-alive");
}

TEST_P(HttpRawRequestTest, one_arg) {
  static constexpr string_view kRequest =
      "PUT /set?somekey=somevalue HTTP/1.1\r\n"
      "Host: localhost:4000 \r\n"
      "User-Agent: Mozilla/5.0\r\n"
      "\r\n";

  const auto result = slurp(kRequest);
  EXPECT_EQ("PUT", result.method);
  EXPECT_EQ("/set", result.path);
  ASSERT_EQ(1, result.path_args.size());
  EXPECT_EQ(result.path_args.at("somekey"), "somevalue");
}

TEST_P(HttpRawRequestTest, two_args) {
  static constexpr string_view kRequest =
      "GET /foo?one=two&three=four HTTP/1.1\r\n"
      "Host: localhost:4000 \r\n"
      "User-Agent: Mozilla/5.0\r\n"
      "\r\n";

  const auto result = slurp(kRequest);
  EXPECT_EQ("GET", result.method);
  EXPECT_EQ("/foo", result.path);
  ASSERT_EQ(2, result.path_args.size());
  EXPECT_EQ(result.path_args.at("one"), "two");
  EXPECT_EQ(result.path_args.at("three"), "four");
}

TEST_P(HttpRawRequestTest, justPut) {
  static constexpr string_view kRequest = "PUT\r\n"
                                          "Host: localhost:4000 \r\n"
                                          "\r\n";

  EXPECT_THROW(slurp(kRequest), Exception);
}

TEST_P(HttpRawRequestTest, justPutAnd) {
  static constexpr string_view kRequest = "PUT /set?somekey=somevalue\r\n"
                                          "Host: localhost:4000 \r\n"
                                          "\r\n";

  EXPECT_THROW(slurp(kRequest), Exception);
}
