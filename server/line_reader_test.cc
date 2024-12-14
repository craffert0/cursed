#include "line_reader.h"

#include "exception.h"
#include "reader.h"

#include "gtest/gtest.h"

#include <deque>
#include <initializer_list>

using namespace std;
using namespace testing;

class FakeReader : public Reader {
public:
  void reset(initializer_list<const char*> strings) {
    strings_ = deque<string>(strings.begin(), strings.end());
  }

  ssize_t read(void* buffer, size_t nbyte) override {
    if (strings_.empty()) {
      return 0;
    }
    auto& r = strings_.front();
    if (nbyte < r.size()) {
      memcpy(buffer, r.data(), nbyte);
      r = r.substr(nbyte);
      return nbyte;
    } else {
      memcpy(buffer, r.data(), r.size());
      const ssize_t retval = r.size();
      strings_.pop_front();
      return retval;
    }
  }

private:
  deque<string> strings_;
};

class LineReaderTest : public Test {
public:
  LineReaderTest() : reader_(fake_) {}

protected:
  FakeReader fake_;
  LineReader reader_;
};

TEST_F(LineReaderTest, basics) {
  fake_.reset({"one\r\n", "two\r\n", "three\r\n"});
  EXPECT_EQ("one", reader_.next());
  EXPECT_EQ("two", reader_.next());
  EXPECT_EQ("three", reader_.next());
  EXPECT_THROW(reader_.next(), Exception);
}

TEST_F(LineReaderTest, breakItUp) {
  fake_.reset({"on", "e\r\n", "t", "wo\r\n", "t", "h", "re", "e\r\n"});
  EXPECT_EQ("one", reader_.next());
  EXPECT_EQ("two", reader_.next());
  EXPECT_EQ("three", reader_.next());
}

TEST_F(LineReaderTest, midCrLf) {
  fake_.reset({"one\r", "\n", "t", "wo\r\n", "t", "h", "re", "e\r\n"});
  EXPECT_EQ("one", reader_.next());
  EXPECT_EQ("two", reader_.next());
  EXPECT_EQ("three", reader_.next());
}

TEST_F(LineReaderTest, allOne) {
  fake_.reset({"one\r\ntwo\r\nthree\r\n"});
  EXPECT_EQ("one", reader_.next());
  EXPECT_EQ("two", reader_.next());
  EXPECT_EQ("three", reader_.next());
  EXPECT_THROW(reader_.next(), Exception);
}

TEST_F(LineReaderTest, lineTooLong) {
  fake_.reset({"0123456789", "0123456789", "0123456789", "0123456789",
               "0123456789", "0123456789", "0123456789", "0123456789",
               "0123456789", "0123456789", "0123456789", "0123456789",
               "0123456789", "0123456789", "0123456789", "0123456789",
               "0123456789", "0123456789", "0123456789", "0123456789",
               "0123456789", "0123456789", "0123456789", "0123456789",
               "0123456789", "0123456789", "0123456789", "0123456789",
               "0123456789", "0123456789", "\r\n"});
  EXPECT_THROW(reader_.next(), Exception);
}
