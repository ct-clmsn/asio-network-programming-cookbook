#include "../common/logging.h"
#include <asio.hpp>

int main() {
  auto console = logging::setup();

  // Step 1. Assume that the client application has already obtained the DNS name and protocol port number and
  // represented them as strings.
  std::string host = "samplehost.com";
  std::string port_num = "3333";

  // Step 2.
  asio::io_service ios;

  // Step 3. Creating a query.
  asio::ip::udp::resolver::query resolver_query(host, port_num,
                                                asio::ip::udp::resolver::query::numeric_service);

  // Step 4. Creating a resolver.
  asio::ip::udp::resolver resolver(ios);

  // Used to store information about error that happens during the resolution process.
  asio::error_code ec;

  // Step 5.
  asio::ip::udp::resolver::iterator it = resolver.resolve(resolver_query, ec);

  // Handling errors if any.
  if (ec.value() != 0) {
    // Failed to resolve the DNS name. Breaking execution.
    console->error("Failed to resolve a DNS name. Error code = {}. Message = {}", ec.value(), ec.message());
    return ec.value();
  }

  asio::ip::udp::resolver::iterator it_end;
  for (; it != it_end; ++it) {
    // Here we can access the endpoint like this.
    auto ep = it->endpoint();
    console->debug("Endpoint: {}", ep);
  }

  return 0;
}
