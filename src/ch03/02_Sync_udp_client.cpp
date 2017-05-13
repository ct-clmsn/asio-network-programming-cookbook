#include "../common/logging.h"
#include <asio.hpp>

class SyncUDPClient {
public:
  SyncUDPClient() : m_sock(m_ios) { m_sock.open(asio::ip::udp::v4()); }
  ~SyncUDPClient() { close(); }

  std::string emulateLongComputationOp(unsigned int duration_sec, const std::string& raw_ip_address,
                                       unsigned short port_num) {
    std::string request = "EMULATE_LONG_COMP_OP " + std::to_string(duration_sec) + "\n";

    asio::ip::udp::endpoint ep{asio::ip::address::from_string(raw_ip_address), port_num};

    sendRequest(ep, request);
    return receiveResponse(ep);
  }

private:
  void close() {
    if (m_sock.is_open()) {
      logging::get()->debug("Shutting down and closing the socket ...");

      // m_sock.shutdown(asio::ip::udp::socket::shutdown_both);
      m_sock.close();
    }
  }

  void sendRequest(const asio::ip::udp::endpoint& ep, const std::string& request) {
    m_sock.send_to(asio::buffer(request), ep);
  }

  std::string receiveResponse(asio::ip::udp::endpoint& ep) {
    char response[6];
    std::size_t bytes_received = m_sock.receive_from(asio::buffer(response), ep);

    return std::string(response, bytes_received);
  }

private:
  asio::io_service m_ios;
  asio::ip::udp::socket m_sock;
};

int main() {
  auto console = logging::setup();

  const std::string server1_raw_ip_address = "127.0.0.1";
  const unsigned short server1_port_num = 3333;

  // const std::string server2_raw_ip_address = "192.168.1.10";
  // const unsigned short server2_port_num = 3334;

  try {
    SyncUDPClient client;

    console->info("Sending request to the server #1 ...");
    std::string response = client.emulateLongComputationOp(10, server1_raw_ip_address, server1_port_num);
    console->info("Response from the server #1 received: {}", response);

    // console->info("Sending request to the server #2 ...");
    // response = client.emulateLongComputationOp(10, server2_raw_ip_address, server2_port_num);
    // console->info("Response from the server #2 received: {}", response);
  } catch (asio::system_error& e) {
    console->error("Error occurred! Error code = {}. Message: {}", e.code().value(), e.what());
    return e.code().value();
  }

  return 0;
}
