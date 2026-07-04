/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include "meta/logger.hpp"

namespace meta
{

std::shared_ptr<spdlog::logger> Logger::instance = nullptr;

std::shared_ptr<spdlog::logger> &Logger::log()
{
  if (!instance)
  {
    instance = spdlog::stdout_color_mt("console_meta");
    instance->set_pattern("[meta--] [%H:%M:%S] [%^---%L---%$] %v");

// Debug / RelWithDebInfo distinction is not directly available in C++,
// so we rely on a CMake-defined macro.
#ifdef META_SPDLOG_LEVEL_TRACE
    constexpr auto level = spdlog::level::trace;
#elif defined(META_SPDLOG_LEVEL_DEBUG)
    constexpr auto level = spdlog::level::debug;
#else
    constexpr auto level = spdlog::level::info;
#endif

    instance->set_level(level);
  }
  return instance;
}

} // namespace meta
