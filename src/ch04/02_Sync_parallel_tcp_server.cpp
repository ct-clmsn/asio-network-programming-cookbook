#include "../common/logging.h"
#include <asio.hpp>
#include <atomic>
#include <memory>
#include <thread>

class Service {
public:
  Service() = default;

  void StartHandlingClient(std::shared_ptr<asio::ip::tcp::socket> sock) {
    std::thread th{[this, sock]() { HandleClient(sock); }};
    th.detach();
  }

private:
  void HandleClient(std::shared_ptr<asio::ip::tcp::socket> sock) {
    try {
      asio::streambuf request;
      asio::read_until(*sock.get(), request, '\n');

      // Emulate request processing.
      int i = 0;
      while (i != 1000000) i++;
      std::this_thread::sleep_for(std::chrono::milliseconds(500));

      // Sending response.
      std::string response = "Response\n";
      asio::write(*sock.get(), asio::buffer(response));
    } catch (asio::system_error& e) {
      logging::get()->error("Error occurred! Error code = {}. Message: {}", e.code().value(), e.what());
    }

    // Clean up
    delete this;
  }
};

class Acceptor {
public:
  Acceptor(asio::io_service& ios, unsigned short port_num)
      : m_ios{ios}, m_acceptor{m_ios, asio::ip::tcp::endpoint{asio::ip::address_v4::any(), port_num}} {
    m_acceptor.listen();
  }

  void Accept() {
    auto sock = std::make_shared<asio::ip::tcp::socket>(m_ios);

    m_acceptor.accept(*sock.get());

    (new Service)->StartHandlingClient(sock);
  }

private:
  asio::io_service& m_ios;
  asio::ip::tcp::acceptor m_acceptor;
};

class Server {
public:
  Server() : m_stop{false} {}

  void Start(unsigned short port_num) {
    m_thread.reset(new std::thread([this, port_num]() { Run(port_num); }));
  }

  void Stop() {
    m_stop.store(true);
    m_thread->join();
  }

private:
  void Run(unsigned short port_num) {
    Acceptor acc{m_ios, port_num};

    while (!m_stop.load()) {
      acc.Accept();
    }
  }

private:
  std::unique_ptr<std::thread> m_thread;
  std::atomic<bool> m_stop;
  asio::io_service m_ios;
};

int main() {
  auto console = logging::setup();

  unsigned short port_num = 3333;

  try {
    Server srv;
    srv.Start(port_num);

    std::this_thread::sleep_for(std::chrono::seconds(60));

    srv.Stop();
  } catch (asio::system_error& e) {
    console->error("Error occurred! Error code = {}. Message: {}", e.code().value(), e.what());
    return e.code().value();
  }

  return 0;
}
