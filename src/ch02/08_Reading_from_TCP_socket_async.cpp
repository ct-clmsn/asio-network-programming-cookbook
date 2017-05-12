#include "../common/logging.h"
#include <asio.hpp>
#include <iostream>

// Keeps objects we need in a callback to identify whether all data has been read from the socket and to
// initiate next async reading operation if needed.
struct Session {
  std::shared_ptr<asio::ip::tcp::socket> sock;
  std::unique_ptr<char[]> buf;
  std::size_t total_bytes_read;
  unsigned int buf_size;
};

// Function used as a callback for asynchronous reading operation. Check if all data has been read from the
// socket and initiates new reading operation if needed.
void callback(const asio::error_code& ec, std::size_t bytes_transferred, std::shared_ptr<Session> s) {
  if (ec.value() != 0) {
    logging::get()->error("Error occurred! Error code = {}. Message: {}", ec.value(), ec.message());
    return;
  }

  s->total_bytes_read += bytes_transferred;
  if (s->total_bytes_read == s->buf_size) {
    logging::get()->info("Total Bytes Read = {}", s->total_bytes_read);
    return;
  }

  s->sock->async_read_some(
      asio::buffer(s->buf.get() + s->total_bytes_read, s->buf_size - s->total_bytes_read),
      std::bind(callback, std::placeholders::_1, std::placeholders::_2, s));
}

void readFromSocket(std::shared_ptr<asio::ip::tcp::socket> sock) {
  auto s = std::make_shared<Session>();

  // Step 4. Allocating the buffer;
  const unsigned int MESSAGE_SIZE = 7;

  s->buf.reset(new char[MESSAGE_SIZE]);
  s->total_bytes_read = 0;
  s->sock = sock;
  s->buf_size = MESSAGE_SIZE;

  // Step 5. Initiating asynchronous reading operation.
  s->sock->async_read_some(asio::buffer(s->buf.get(), s->buf_size),
                           std::bind(callback, std::placeholders::_1, std::placeholders::_2, s));
}

int main() {
  auto console = logging::setup();

  std::string raw_ip_address = "127.0.0.1";
  unsigned short port_num = 3333;

  try {
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);
    asio::io_service ios;

    // Step 3. Allocating, opening and connecting a socket.
    auto sock = std::make_shared<asio::ip::tcp::socket>(ios, ep.protocol());
    sock->connect(ep);

    readFromSocket(sock);

    // Step 6.
    ios.run();

  } catch (asio::system_error& e) {
    console->error("Error occurred! Error code = {}. Message: {}", e.code(), e.what());
    return e.code().value();
  }

  return 0;
}
