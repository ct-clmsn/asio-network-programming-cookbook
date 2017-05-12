#include "../common/logging.h"
#include <asio.hpp>
#include <iostream>
#include <memory>

// Keeps objects we need in a callback to identify whether all data has been written to the socket and to
// initiate next async writing operation if needed.
struct Session {
  std::shared_ptr<asio::ip::tcp::socket> sock;
  std::string buf;
  std::size_t total_bytes_written;
};

// Function used as a callback for asynchronous writing operation. Checks if all data from the buffer has been
// written to the socket and initiates new asynchronous writing operation if needed.
void callback(const asio::error_code& ec, std::size_t bytes_transferred, std::shared_ptr<Session> s) {
  if (ec.value() != 0) {
    logging::get()->error("Error occurred! Error code = {}. Message: {}", ec.value(), ec.message());
    return;
  }

  s->total_bytes_written += bytes_transferred;
  if (s->total_bytes_written == s->buf.length()) {
    return;
  }

  s->sock->async_write_some(
      asio::buffer(s->buf.c_str() + s->total_bytes_written, s->buf.length() - s->total_bytes_written),
      std::bind(callback, std::placeholders::_1, std::placeholders::_2, s));
}

void writeToSocket(std::shared_ptr<asio::ip::tcp::socket> sock) {
  // We need the Session object allocated it in the free memory and not on the stack; it must live until the
  // callback function is called.
  auto s = std::make_shared<Session>();

  // Step 4. Allocating and filling the buffer.
  s->buf = std::string("Hello\n");
  s->total_bytes_written = 0;
  s->sock = sock;

  // Step 5. Initiating asynchronous write operation.
  s->sock->async_write_some(asio::buffer(s->buf),
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

    writeToSocket(sock);

    // Step 6.
    // The run() method blocks, as long as, at least one pending asynchronous operation. When the last
    // callback of the last pending asynchronous operation is completed, this method returns.
    ios.run();
  } catch (asio::system_error& e) {
    console->error("Error occurred! Error code = {}. Message: {}", e.code().value(), e.what());
    return e.code().value();
  }

  return 0;
}
