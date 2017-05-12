// The client application:
//   The purpose of the client application is to allocate the socket and connect it to the server application.
//   After the connection is established, the application should prepare and send a request message notifying
//   its boundary by shutting down the socket after writing the message to it.
//
//   After the request is sent, the client application should read the response. The size of the response is
//   unknown; therefore, the reading should be performed until the server closes its socket to notify the
//   response boundary.

#include "../common/buffer.h"
#include "../common/logging.h"
#include <asio.hpp>

// We define a function that accepts a reference to the socket object connected to the server and performs the
// communication with the server using this socket.
void communicate(asio::ip::tcp::socket& sock) {
  // Allocating and filling the buffer with binary data.
  const char request_buf[] = {0x48, 0x65, 0x0, 0x6c, 0x6c, 0x6f};

  // Sending the request data.
  asio::write(sock, asio::buffer(request_buf));

  // Shutting down the socket to let the server know that we've sent the whole request.
  sock.shutdown(asio::socket_base::shutdown_send);

  // We use extensible buffer for response because we don't know the size of the response message.
  asio::streambuf response_buf;
  asio::error_code ec;

  asio::read(sock, response_buf, ec);
  if (ec == asio::error::eof) {
    // Whole response message has been received.
    // Here we can handle it.
    logging::get()->info("Response Received: {}", buffer_to_string(response_buf));
  } else {
    throw asio::system_error(ec);
  }
}

int main() {
  auto console = logging::setup();

  std::string raw_ip_address = "127.0.0.1";
  unsigned short port_num = 3333;

  try {
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);
    asio::io_service ios;

    asio::ip::tcp::socket sock(ios, ep.protocol());
    sock.connect(ep);

    communicate(sock);
  } catch (asio::system_error& e) {
    console->error("Error occurred! Error code = {}. Message: {}", e.code().value(), e.what());
    return e.code().value();
  }

  return 0;
}
