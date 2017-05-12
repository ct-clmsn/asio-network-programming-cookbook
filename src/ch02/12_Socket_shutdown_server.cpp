// The server application :
//
// The server application is intended to allocate an acceptor socket and passively wait for a connection
// request. When the connection request arrives, it should accept it and read the data from the socket
// connected to the client until the client application shuts down the socket on its side. Having received the
// request message, the server application should send the response message notifying its boundary by shutting
// down the socket.

#include "../common/buffer.h"
#include "../common/logging.h"
#include <asio.hpp>

void processRequest(asio::ip::tcp::socket& sock) {
  // We use extensible buffer because we don't know the size of the request message.
  asio::streambuf request_buf;
  asio::error_code ec;

  // Receiving the request.
  asio::read(sock, request_buf, ec);

  if (ec != asio::error::eof) {
    throw asio::system_error(ec);
  }

  // Request received.
  logging::get()->info("Request received: {}", buffer_to_string(request_buf));
  logging::get()->info("Sending response...");

  // Sending response. Allocating and filling the buffer with binary data.
  const char response_buf[] = {0x48, 0x69, 0x21};

  // Sending the request data.
  asio::write(sock, asio::buffer(response_buf));

  // Shutting down the socket to let the client know that we've sent the whole response.
  sock.shutdown(asio::socket_base::shutdown_send);
}

int main() {
  auto console = logging::setup();

  unsigned short port_num = 3333;

  try {
    asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port_num);
    asio::io_service ios;

    asio::ip::tcp::acceptor acceptor(ios, ep);
    asio::ip::tcp::socket sock(ios);

    acceptor.accept(sock);

    processRequest(sock);
  } catch (asio::system_error& e) {
    console->error("Error occurred! Error code = {}. Message: {}", e.code().value(), e.what());
    return e.code().value();
  }

  return 0;
}
