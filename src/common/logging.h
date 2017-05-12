#ifndef LOGGING_H
#define LOGGING_H

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

  auto get() { return spdlog::get("console"); }

  void teardown() { spdlog::drop_all(); }
}

#endif /* LOGGING_H */
