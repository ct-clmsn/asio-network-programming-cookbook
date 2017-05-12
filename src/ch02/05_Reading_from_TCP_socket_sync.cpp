#include "../common/logging.h"
#include <asio.hpp>
#include <iostream>

/*
 * First run a server with: nc -l 3333
 * Then run this application as client to the previous server.
 * Write something on the server console and hit RETURN 2 times to send the data to the client.
 */

std::string readFromSocket(asio::ip::tcp::socket& sock) {
  const unsigned char MESSAGE_SIZE = 7;
  char buf[MESSAGE_SIZE];
  std::size_t total_bytes_read = 0;

  while (total_bytes_read != MESSAGE_SIZE) {
    total_bytes_read += sock.read_some(asio::buffer(buf + total_bytes_read, MESSAGE_SIZE - total_bytes_read));
  }

  return std::string(buf, total_bytes_read);
}

std::string readFromSocketUsingReceive(asio::ip::tcp::socket& sock) {
  const unsigned char MESSAGE_SIZE = 7;
  char buf[MESSAGE_SIZE];
  std::size_t total_bytes_read = 0;

  while (total_bytes_read != MESSAGE_SIZE) {
    total_bytes_read += sock.receive(asio::buffer(buf + total_bytes_read, MESSAGE_SIZE - total_bytes_read));
  }

  return std::string(buf, total_bytes_read);
}

std::string readFromSocketEnhanced(asio::ip::tcp::socket& sock) {
  const unsigned char MESSAGE_SIZE = 7;
  char buf[MESSAGE_SIZE];

  // will block the thread of execution until exactly 7 bytes are read or an error occurs.
  asio::read(sock, asio::buffer(buf, MESSAGE_SIZE));
  return std::string(buf, MESSAGE_SIZE);
}

std::string readFromSocketDelim(asio::ip::tcp::socket& sock) {
  asio::streambuf buf;

  // Synchronously read data from the socket until '\n' symbol is encountered.
  asio::read_until(sock, buf, '|');

  // Because buffer 'buf' may contain some other data after '\n' symbol, we have to parse the buffer and
  // extract only symbols before the delimiter.
  std::string message;
  std::istream input_stream(&buf);
  std::getline(input_stream, message, '|');
  return message;
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

    // std::string data = readFromSocket(sock);
    // std::string data = readFromSocketUsingReceive(sock);
    // std::string data = readFromSocketEnhanced(sock);
    std::string data = readFromSocketDelim(sock);
    console->info("Data read: {}", data);
  } catch (asio::system_error& e) {
    console->error("Error occurred! Error code = {}. Message: {}", e.code(), e.what());
    return e.code().value();
  }

  return 0;
}
