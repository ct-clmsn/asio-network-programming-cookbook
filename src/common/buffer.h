#ifndef BUFFER_H
#define BUFFER_H

#include <asio.hpp>
#include <string>

// Utility function to convert an asio::streambuf to std::string
std::string buffer_to_string(const asio::streambuf& buffer) {
  using asio::buffers_begin;

  auto bufs = buffer.data();
  std::string result(buffers_begin(bufs), buffers_begin(bufs) + buffer.size());
  return result;
}

#endif /* BUFFER_H */
