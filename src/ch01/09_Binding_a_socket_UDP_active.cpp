#include "../common/logging.h"
#include <asio.hpp>

int main() {
  auto console = logging::setup();

  // Step 1. Here we assume that the server application has already obtained the protocol port number.
  unsigned short port_num = 3333;

  // Step 2. Creating an endpoint.
  asio::ip::udp::endpoint ep(asio::ip::address_v4::any(), port_num);

  // Used by 'acceptor' class constructor
  asio::io_service ios;

  // Step 3. Creating and opening a socket.
  asio::ip::udp::socket sock(ios, ep.protocol());

  // Errors
  asio::error_code ec;

  // Step 4. Binding the socket to an endpoint
  sock.bind(ep, ec);

  // Handling errors if any.
  if (ec.value() != 0) {
    // Failed to bind the socket. Breaking execution.
    console->error("Failed to bind the socket. Error code = {}. Message: {}", ec.value(), ec.message());
    return ec.value();
  }

  console->info(
      "Socket successfully bound to '{}'. Ready to start listening for incoming connection "
      "requests on that endpoint.",
      ep);

  return 0;
}
