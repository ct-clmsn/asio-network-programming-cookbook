#include <iostream>
#include <memory>
#include <spdlog/spdlog.h>

namespace logging {
  auto setup() {
    spdlog::set_pattern("[%n.%l] >> %v");

#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::info);
#endif

    auto console = spdlog::stdout_logger_mt("console");
    return console;
  }

  void teardown() { spdlog::drop_all(); }
}
