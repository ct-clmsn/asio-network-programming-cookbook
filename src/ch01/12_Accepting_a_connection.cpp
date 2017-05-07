#include "../common/logging.h"
#include <asio.hpp>

int main() {
  auto console = logging::setup();

  // The size of the queue containing the pending connection requests.
  const int BACKLOG_SIZE = 30;

  // Step 1. Here we assume that the server application has already obtained the protocol port number.
  unsigned short port_num = 3333;

  // Step 2. Creating a server endpoint.
  asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port_num);

  asio::io_service ios;

  try {
    // Step 3. Instantiating and opening an acceptor socket.
    asio::ip::tcp::acceptor acceptor(ios, ep.protocol());

    // Step 4. Binding the acceptor socket to the server endpoint.
    acceptor.bind(ep);

    // Step 5. Starting to listen for incoming connection requests.
    acceptor.listen(BACKLOG_SIZE);

    // Step 6. Creating an active socket.
    asio::ip::tcp::socket sock(ios);

    // Step 7. Processing the next connection request and connecting the active socket to the client.
    acceptor.accept(sock);

    // At this point 'sock' socket is connected to the client application and can be used to send data to or
    // receive data from it.
    console->info("Connection accepted. Connected to client.");
  } catch (asio::system_error& e) {
    console->error("Error occurred! Error code = {}. Message: ", e.code(), e.what());
    return e.code().value();
  }

  return 0;
}
