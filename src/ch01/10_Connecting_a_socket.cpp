#include "../common/logging.h"
#include <asio.hpp>

int main() {
  auto console = logging::setup();

  // Step 1. Assume that the client application has already obtained the IP address and protocol port number
  // of the target server.
  std::string raw_ip_address = "127.0.0.1";
  unsigned short port_num = 3333;

  try {
    // Step 2. Creating an endpoint designating a target server application.
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);

    asio::io_service ios;

    // Step 3. Creating an opening a socket.
    asio::ip::tcp::socket sock(ios, ep.protocol());

    // Step 4. Connecting a socket.
    sock.connect(ep);

    // At this point socket 'sock' is connected to the server application and can be used to send data to or
    // receive data from it.
    console->info("Socket is connected to the server!");
  }
  // Overloads of asio::ip::address::from_string() and asio::ip::tcp::socket::connect() used here throw
  // exceptions in case of error condition.
  catch (asio::system_error& e) {
    console->error("Error occurred! Error code = {}. Message: {}", e.code(), e.what());
    return e.code().value();
  }

  return 0;
}
