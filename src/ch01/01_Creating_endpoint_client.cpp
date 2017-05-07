#include "../common/logging.h"
#include <asio.hpp>

int main() {
  auto console = logging::setup();

  // Step 1. Assume that the client application has already obtained the IP-address and the protocol port
  // number.
  std::string raw_ip_address = "127.0.0.1";
  unsigned short port_number = 3333;

  // Used to store information about error that happens while parsing the raw IP-address.
  asio::error_code ec;

  // Step 2. Using IP protocol version independen address representation
  asio::ip::address ip_address = asio::ip::address::from_string(raw_ip_address, ec);
  if (ec.value() != 0) {
    // Provided IP address is invalid. Breaking execution.
    console->error("Failed to parse the IP address. Error code = {}. Message: {}", ec.value(), ec.message());
    return ec.value();
  }

  // Step 3.
  asio::ip::tcp::endpoint ep(ip_address, port_number);
  console->debug("Here's the endpoint: {}", ep);

  // Step 4. The endpoint is ready and can be used to specify a particular server in the network the client
  // wants to communicate with.

  return 0;
}
