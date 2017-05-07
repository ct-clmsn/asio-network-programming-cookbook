#include "../common/logging.h"
#include <asio.hpp>

int main() {
  auto console = logging::setup();

  // Step 1. Here we assume that the server application has already obtained the protocol port number.
  unsigned short port_num = 3333;

  // Step 2. Creating an endpoint.
  asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port_num);

  // Used by 'acceptor' class constructor
  asio::io_service ios;

  // Step 3. Creating and opening an acceptor socket.
  asio::ip::tcp::acceptor acceptor(ios, ep.protocol());

  // Errors
  asio::error_code ec;

  // Step 4. Binding the acceptor socket.
  acceptor.bind(ep, ec);

  // Handling errors if any.
  if (ec.value() != 0) {
    // Failed to bind the acceptor socket. Breaking execution.
    console->error("Failed to bind the acceptor socket. Error code = {}. Message: {}", ec.value(),
                   ec.message());
    return ec.value();
  }

  console->info(
      "Acceptor socket successfully bound to '{}'. Ready to start listening for incoming connection "
      "requests on that endpoint.",
      ep);

  return 0;
}
