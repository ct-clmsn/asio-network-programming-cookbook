#include "../common/logging.h"
#include <asio.hpp>
#include <chrono>
#include <memory>
#include <thread>

int main() {
  auto console = logging::setup();

  std::string raw_ip_address = "127.0.0.1";
  unsigned short port_num = 3333;

  try {
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);
    asio::io_service ios;

    auto sock = std::make_shared<asio::ip::tcp::socket>(ios, ep.protocol());
    sock->async_connect(ep, [sock, console](const asio::error_code& ec) {
      // If asynchronous operation has been cancelled or an error occurred during execution, ec contains
      // corresponding error code.
      if (ec.value() != 0) {
        if (ec == asio::error::operation_aborted) {
          console->info("Operation canceled!");
        } else {
          console->error("Error occurred! Error code = {}. Message: {}", ec.value(), ec.message());
        }

        return;
      }

      // At this point the socket is connected and can
      // be used for communication with remote application.
      console->info("Socket connected!!");
    });

    // Starting a thread, which will be used to call the callback when asynchronous operation completes.
    std::thread worker_thread([&ios, console]() {
      try {
        ios.run();
      } catch (asio::system_error& e) {
        console->error("Error occurred! Error code = {}. Message: {}", e.code().value(), e.what());
      }
    });

    // Emulating delay.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Cancelling the initiated operation.
    sock->cancel();

    // Waiting for the worker thread to complete.
    worker_thread.join();
  } catch (asio::system_error& e) {
    console->error("Error occurred! Error code = {}. Message: {}", e.code().value(), e.what());
    return e.code().value();
  }

  return 0;
}
