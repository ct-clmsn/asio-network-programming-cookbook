#include "../common/logging.h"
#include <asio.hpp>
#include <iostream>

int main() {
  auto console = logging::setup();

  asio::streambuf buf;
  std::ostream output(&buf);

  // Writing the message to the stream-based buffer.
  // Usually, in a typical client or server application, the data will be written to the buf stream buffer by
  // the Boost.Asio input function such as asio::read(), which accepts a stream buffer object as an argument
  // and reads data from the socket to that buffer.
  output << "Message1\nMessage2";

  // Now we want to read all data from a streambuf until '\n' delimiter. Instantiate an input stream which
  // uses our stream buffer.
  std::istream input(&buf);

  // We'll read data into this string.
  std::string message1;
  std::getline(input, message1);

  // Now message1 string contains 'Message1'.
  console->info("Message: {}", message1);

  return 0;
}
