#include "../common/logging.h"
#include <asio.hpp>

int main() {
  auto console = logging::setup();

  std::string buf;  // 'buf' is the raw buffer.
  buf = "Hello";    // Step 1 and 2 in single line.

  // Step 3. Creating buffer representation that satisfies ConstBufferSequence concept requirements.
  asio::const_buffers_1 output_buf = asio::buffer(buf);
  (void)output_buf;

  // Step 4. 'output_buf' is the representation of the buffer 'buf' that can be sued in Boost.Asio output
  // operations.

  return 0;
}
