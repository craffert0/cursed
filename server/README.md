## Exception

Wraps an HTTP error code and message. With the convenience macro `ASSERT(condition, code, what)`, allows more declarative code for handling an HTTP request, with a top-level `catch` of that exception.

## Reader, Writer, and Socket

`Reader` and `Writer` are abstract classes with similar APIs as `read(2)` and `write(2)` (respectively). `Socket` concretely implements them wrapping an `int` and calling those functions.

## LineReader

Iterator that wraps a `Reader` and splits its CRLF separated output into individual `string_views`. Very convenient.

## HttpRawRequest

A Blob that holds all the pieces of an HTTP Request. `slurp_raw_request()` parses the input from the socket to produce an instance

## Server

The underlying database server. It's single method `serve(sock)` reads, processes, and responds to the HTTP request.

This is the bit that will need to be updated to do persistent storage.

## main

Creates, binds, and listens to the socket, and passes off each accepted connection to the `Server`.
