#include "../common/logging.h"
#include <asio.hpp>
#include <iostream>

/*
 * First run a server with: nc -l 3333
 * Then run this application as client to the previous server.
 */

void writeToSocket(asio::ip::tcp::socket& sock) {
  // Step 2. Allocating and filling the buffer.
  std::string buf = "Hello\n";
  std::size_t total_bytes_written = 0;

  // Step 3. Run the loop until all data is written to the socket.
  while (total_bytes_written != buf.length()) {
    // Each time the write_some() method is called, the argument passed to it is adjusted. The start byte of
    // the buffer is shifted by the value of total_bytes_written as compared to the original buffer (because
    // the previous bytes have already been sent by preceding calls to the write_some() method) and the size
    // of the buffer is decreased by the same value, correspondingly.
    total_bytes_written +=
        sock.write_some(asio::buffer(buf.c_str() + total_bytes_written, buf.length() - total_bytes_written));
  }
}

void writeToSocketUsingSend(asio::ip::tcp::socket& sock) {
  // Step 2. Allocating and filling the buffer.
  std::string buf = "Hello\n";
  std::size_t total_bytes_written = 0;

  // Step 3. Run the loop until all data is written to the socket.
  while (total_bytes_written != buf.length()) {
    total_bytes_written +=
        sock.send(asio::buffer(buf.c_str() + total_bytes_written, buf.length() - total_bytes_written));
  }
}

void writeToSocketEnhanced(asio::ip::tcp::socket& sock) {
  // Allocating and filling the buffer.
  std::string buf = "Hello\n";

  // Write whole buffer to the socket.
  asio::write(sock, asio::buffer(buf));
}

int main() {
  auto console = logging::setup();

  std::string raw_ip_address = "127.0.0.1";
  unsigned short port_num = 3333;

  try {
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);
    asio::io_service ios;

    // Step 1. Allocating and opening the socket.
    asio::ip::tcp::socket sock(ios, ep.protocol());
    sock.connect(ep);

    // writeToSocket(sock);
    // writeToSocketUsingSend(sock);
    writeToSocketEnhanced(sock);
  } catch (asio::system_error& e) {
    console->error("Error occurred! Error code = {}. Message: {}", e.code(), e.what());
    return e.code().value();
  }

  console->info("Data written to socket!");

  return 0;
}
