cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
        ":server",
    ],
)

cc_library(
    name = "server",
    srcs = ["server.cc"],
    hdrs = ["server.h"],
    deps = [
        ":exception",
        ":http_raw_request",
        ":socket",
    ],
)

cc_library(
    name = "exception",
    hdrs = ["exception.h"]
)

cc_library(
    name = "reader",
    hdrs = ["reader.h"]
)

cc_library(
    name = "writer",
    hdrs = ["writer.h"]
)

cc_library(
    name = "socket",
    srcs = ["socket.cc"],
    hdrs = ["socket.h"],
    deps = [
        ":reader",
        ":writer",
    ],
)

cc_library(
    name = "line_reader",
    srcs = ["line_reader.cc"],
    hdrs = ["line_reader.h"],
    deps = [
        ":exception",
        ":reader",
    ],
)

cc_test(
    name = "line_reader_test",
    srcs = ["line_reader_test.cc"],
    size = "small",
    deps = [
        ":line_reader",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "http_raw_request",
    srcs = ["http_raw_request.cc"],
    hdrs = ["http_raw_request.h"],
    deps = [
        ":exception",
        ":line_reader",
    ]
)

cc_test(
    name = "http_raw_request_test",
    srcs = ["http_raw_request_test.cc"],
    size = "small",
    deps = [
        ":http_raw_request",
        "@googletest//:gtest_main",
    ],
)
