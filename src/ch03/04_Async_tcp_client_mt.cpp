#include "../common/logging.h"
#include <asio.hpp>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

// Function pointer type that points to the callback function which is called when a request is complete.
typedef void (*Callback)(unsigned int request_id, const std::string& response, const asio::error_code& ec);

// Structure represents a context of a single request.
struct Session {
  Session(asio::io_service& ios, const std::string& raw_ip_address, unsigned short port_num,
          const std::string& request, unsigned int id, Callback callback)
      : m_sock{ios},
        m_ep{asio::ip::address::from_string(raw_ip_address), port_num},
        m_request{request},
        m_id{id},
        m_callback{callback},
        m_was_cancelled{false} {}

  asio::ip::tcp::socket m_sock;  // Socket used for communication
  asio::ip::tcp::endpoint m_ep;  // Remote endpoint.
  std::string m_request;         // Request string;

  asio::streambuf m_response_buf;  // streambuf where the response will be stored.
  std::string m_response;          // Response represented as a string.

  // Contains the description of an error if one occurs during the request lifecycle.
  asio::error_code m_ec;

  unsigned int m_id;  // Unique ID assigned to the request.

  // Pointer to the function to be called when the request completes.
  Callback m_callback;

  bool m_was_cancelled;
  std::mutex m_cancel_guard;
};

class AsyncTCPClient {
public:
  AsyncTCPClient(unsigned char num_of_threads) {
    m_work.reset(new asio::io_service::work{m_ios});

    for (unsigned char i = 1; i <= num_of_threads; ++i) {
      std::unique_ptr<std::thread> th{new std::thread{[this]() { m_ios.run(); }}};
      m_threads.push_back(std::move(th));
    }
  }
  ~AsyncTCPClient() = default;
  AsyncTCPClient(const AsyncTCPClient& src) = delete;
  AsyncTCPClient(AsyncTCPClient&& src) noexcept = delete;
  AsyncTCPClient& operator=(const AsyncTCPClient& rhs) = delete;
  AsyncTCPClient& operator=(AsyncTCPClient&& rhs) noexcept = delete;

  void emulateLongComputationOp(unsigned int duration_sec, const std::string& raw_ip_address,
                                unsigned short port_num, Callback callback, unsigned int request_id) {
    // Preparing the request string.
    std::string request = "EMULATE_LONG_CALC_OP " + std::to_string(duration_sec) + "\n";

    auto session = std::make_shared<Session>(m_ios, raw_ip_address, port_num, request, request_id, callback);
    session->m_sock.open(session->m_ep.protocol());

    {  // Add new session to the list of active sessions so that we can access it if the user decides to
       // cancel the corresponding request before it completes. Because active sessions list can be accessed
       // from multiple threads, we guard it with a mutex to avoid data corruption.
      std::unique_lock<std::mutex> lock{m_active_sessions_guard};
      m_active_sessions[request_id] = session;
    }

    session->m_sock.async_connect(session->m_ep, [this, session](const asio::error_code& connect_ec) {
      if (connect_ec.value() != 0) {
        session->m_ec = connect_ec;
        onRequestComplete(session);
        return;
      }

      {
        std::unique_lock<std::mutex> cancel_lock{session->m_cancel_guard};
        if (session->m_was_cancelled) {
          onRequestComplete(session);
          return;
        }
      }

      asio::async_write(
          session->m_sock, asio::buffer(session->m_request),
          [this, session](const asio::error_code& write_ec, std::size_t /* bytes_transferred */) {
            if (write_ec.value() != 0) {
              session->m_ec = write_ec;
              onRequestComplete(session);
              return;
            }

            {
              std::unique_lock<std::mutex> cancel_lock{session->m_cancel_guard};
              if (session->m_was_cancelled) {
                onRequestComplete(session);
                return;
              }
            }

            asio::async_read_until(
                session->m_sock, session->m_response_buf, '\n',
                [this, session](const asio::error_code& read_ec, std::size_t /* bytes_transferred */) {
                  if (read_ec.value() != 0) {
                    session->m_ec = read_ec;
                  } else {
                    std::istream strm{&session->m_response_buf};
                    std::getline(strm, session->m_response);
                  }

                  onRequestComplete(session);
                });
          });
    });
  }

  // Cancels the request.
  void cancelRequest(unsigned int request_id) {
    std::unique_lock<std::mutex> lock{m_active_sessions_guard};

    auto it = m_active_sessions.find(request_id);
    if (it != m_active_sessions.end()) {
      std::unique_lock<std::mutex> cancel_lock{it->second->m_cancel_guard};

      it->second->m_was_cancelled = true;
      it->second->m_sock.cancel();
    }
  }

  void close() {
    // Destroy work object. This allows the I/O thread to
    // exits the event loop when there are no more pending
    // asynchronous operations.
    m_work.reset(nullptr);

    // Waiting for the I/O threads to exit.
    for (auto& thread : m_threads) {
      thread->join();
    }
  }

private:
  void onRequestComplete(std::shared_ptr<Session> session) {
    // Shutting down the connection. This method may fail in case socket is not connected. We don't care about
    // the error code if this function fails.
    asio::error_code ignored_ec;
    session->m_sock.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);

    {  // Remove session from the map of active sessions.
      std::unique_lock<std::mutex> lock{m_active_sessions_guard};

      auto it = m_active_sessions.find(session->m_id);
      if (it != m_active_sessions.end()) {
        m_active_sessions.erase(it);
      }
    }

    asio::error_code ec;
    if (session->m_ec.value() == 0 && session->m_was_cancelled) {
      ec = asio::error::operation_aborted;
    } else {
      ec = session->m_ec;
    }

    // Call the callback provided by the user.
    session->m_callback(session->m_id, session->m_response, ec);
  }

private:
  asio::io_service m_ios;
  std::map<int, std::shared_ptr<Session>> m_active_sessions;
  std::mutex m_active_sessions_guard;
  std::unique_ptr<asio::io_service::work> m_work;
  std::list<std::unique_ptr<std::thread>> m_threads;
};

void handler(unsigned int request_id, const std::string& response, const asio::error_code& ec) {
  if (ec.value() == 0) {
    logging::get()->info("Request #{} has completed. Response: {}", request_id, response);
  } else if (ec == asio::error::operation_aborted) {
    logging::get()->info("Request #{} has been cancelled by the user.", request_id);
  } else {
    logging::get()->error("Request #{} failed! Error code = {}. Error message: {}", request_id, ec.value(),
                          ec.message());
  }

  return;
}

int main() {
  auto console = logging::setup();

  try {
    AsyncTCPClient client{4};

    // Here we emulate the user's behavior ...

    // User initiates a request with id 1.
    client.emulateLongComputationOp(10, "127.0.0.1", 3333, handler, 1);

    // Then does nothing for 5 seconds.
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Then initiates another request with id 2.
    client.emulateLongComputationOp(11, "127.0.0.1", 3334, handler, 2);

    // Then decides to cancel the request with id 1.
    client.cancelRequest(1);

    // Does nothing for another 6 seconds.
    std::this_thread::sleep_for(std::chrono::seconds(6));

    // Initiates one more request assigning ID 3 to it.
    client.emulateLongComputationOp(12, "127.0.0.1", 3335, handler, 3);

    // Does nothing for another 15 seconds.
    std::this_thread::sleep_for(std::chrono::seconds(15));

    // Decides to exit the application.
    client.close();
  } catch (asio::system_error& e) {
    console->error("Error occurred! Error code = {}. Message: {}", e.code().value(), e.what());
    return e.code().value();
  }

  return 0;
}
