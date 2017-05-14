#include "../common/logging.h"
#include <asio.hpp>
#include <atomic>
#include <cassert>
#include <memory>
#include <thread>

class Service {
public:
  Service(std::shared_ptr<asio::ip::tcp::socket> sock) : m_sock{sock} {}

  void StartHandling() {
    asio::async_read_until(*m_sock.get(), m_request, '\n',
                           [this](const asio::error_code& ec, std::size_t bytes_transferred) {
                             onRequestReceived(ec, bytes_transferred);
                           });
  }

private:
  void onRequestReceived(const asio::error_code& ec, std::size_t /* bytes_transferred */) {
    if (ec.value() != 0) {
      logging::get()->error("Error occurred! Error code = {}. Message: {}", ec.value(), ec.message());
      onFinish();
      return;
    }

    // Process the request.
    m_response = ProcessRequest(m_request);

    // Initiate asynchronous write operation.
    asio::async_write(*m_sock.get(), asio::buffer(m_response),
                      [this](const asio::error_code& write_ec, std::size_t write_bytes_transferred) {
                        onResponseSent(write_ec, write_bytes_transferred);
                      });
  }

  void onResponseSent(const asio::error_code& ec, std::size_t /* bytes_transferred */) {
    if (ec.value() != 0) {
      logging::get()->error("Error occurred! Error code = {}. Message: {}", ec.value(), ec.message());
    }

    onFinish();
  }

  // Here we perform the cleanup.
  void onFinish() { delete this; }

  std::string ProcessRequest(asio::streambuf& /* request */) {
    // In this method we parse the request, process it and prepare the response.

    // emulate CPU-consuming operations.
    int i = 0;
    while (i != 1000000) i++;

    // Emulate operations that block the thread (e.g. sync I/O operations).
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Prepare and return the response message.
    std::string response = "Response\n";
    return response;
  }

private:
  std::shared_ptr<asio::ip::tcp::socket> m_sock;
  std::string m_response;
  asio::streambuf m_request;
};

class Acceptor {
public:
  Acceptor(asio::io_service& ios, unsigned short port_num)
      : m_ios{ios},
        m_acceptor{m_ios, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), port_num)},
        m_isStopped{false} {}

  // Start accepting incoming connection requests.
  void Start() {
    m_acceptor.listen();
    InitAccept();
  }

  // Stop accepting incomming connection requests.
  void Stop() { m_isStopped.store(true); }

private:
  void InitAccept() {
    auto sock = std::make_shared<asio::ip::tcp::socket>(m_ios);

    m_acceptor.async_accept(*sock.get(), [this, sock](const asio::error_code& ec) { onAccept(ec, sock); });
  }

  void onAccept(const asio::error_code& ec, std::shared_ptr<asio::ip::tcp::socket> sock) {
    if (ec.value() == 0) {
      (new Service(sock))->StartHandling();
    } else {
      logging::get()->error("Error occured! Error code = {}. Message: {}", ec.value(), ec.message());
    }

    // Init next async accept operation if acceptor has not been stopped yet.
    if (!m_isStopped.load()) {
      InitAccept();
    } else {
      // Stop accepting incoming connections and free allocated resourses.
      m_acceptor.close();
    }
  }

private:
  asio::io_service& m_ios;
  asio::ip::tcp::acceptor m_acceptor;
  std::atomic<bool> m_isStopped;
};

class Server {
public:
  Server() { m_work.reset(new asio::io_service::work(m_ios)); }

  // Start the server.
  void Start(unsigned short port_num, unsigned int thread_pool_size) {
    assert(thread_pool_size > 0);

    // create and start Acceptor.
    acc.reset(new Acceptor(m_ios, port_num));
    acc->Start();

    // Create specified number of threads and add them to the pool.
    for (unsigned int i = 0; i < thread_pool_size; ++i) {
      std::unique_ptr<std::thread> th(new std::thread([this]() { m_ios.run(); }));
      m_thread_pool.push_back(std::move(th));
    }
  }

  // Stop the server.
  void Stop() {
    acc->Stop();
    m_ios.stop();

    for (auto& th : m_thread_pool) {
      th->join();
    }
  }

private:
  asio::io_service m_ios;
  std::unique_ptr<asio::io_service::work> m_work;
  std::unique_ptr<Acceptor> acc;
  std::vector<std::unique_ptr<std::thread>> m_thread_pool;
};

const unsigned int DEFAULT_THREAD_POOL_SIZE = 2;

int main() {
  auto console = logging::setup();

  unsigned short port_num = 3333;

  try {
    Server srv;

    unsigned int thread_pool_size = std::thread::hardware_concurrency() * 2;
    if (thread_pool_size == 0) thread_pool_size = DEFAULT_THREAD_POOL_SIZE;

    srv.Start(port_num, thread_pool_size);

    std::this_thread::sleep_for(std::chrono::seconds(60));

    srv.Stop();
  } catch (asio::system_error& e) {
    logging::get()->error("Error occured! Error code = {}. Message: {}", e.code().value(), e.what());
    return e.code().value();
  }

  return 0;
}
