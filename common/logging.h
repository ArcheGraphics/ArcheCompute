//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

#include <spdlog/spdlog.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include <functional>
#include <fmt/format.h>

namespace vox {

using log_level = spdlog::level::level_enum;

namespace detail {
[[nodiscard]] spdlog::logger &default_logger() noexcept;
void set_sink(spdlog::sink_ptr sink) noexcept;
spdlog::sink_ptr create_sink_with_callback(
    std::function<void(const char *level,
                       const char *message)>
        callback) noexcept;
}// namespace detail

template<typename... Args>
inline void log_verbose(Args &&...args) noexcept {
    detail::default_logger().debug(std::forward<Args>(args)...);
}

template<typename... Args>
inline void log_info(Args &&...args) noexcept {
    detail::default_logger().info(std::forward<Args>(args)...);
}

template<typename... Args>
inline void log_warning(Args &&...args) noexcept {
    detail::default_logger().warn(std::forward<Args>(args)...);
}

template<typename... Args>
[[noreturn]] void log_error(Args &&...args) noexcept {
    auto error_message = fmt::format(std::forward<Args>(args)...);
    detail::default_logger().error("{}", error_message);
    std::abort();
}

/// Set log level as verbose
void log_level_verbose() noexcept;
/// Set log level as info
void log_level_info() noexcept;
/// Set log level as warning
void log_level_warning() noexcept;
/// Set log level as error
void log_level_error() noexcept;

/// flush the logs
void log_flush() noexcept;

}// namespace vox

/**
 * @brief Verbose logging
 * 
 * Ex. VERBOSE("function {} returns {}", functionName, functionReturnInt);
 */
#define VERBOSE(fmt, ...) \
    ::vox::log_verbose(FMT_STRING(fmt) __VA_OPT__(, ) __VA_ARGS__)
/**
 * @brief Info logging
 * 
 * Ex. INFO("function {} returns {}", functionName, functionReturnInt);
 */
#define INFO(fmt, ...) \
    ::vox::log_info(FMT_STRING(fmt) __VA_OPT__(, ) __VA_ARGS__)
/**
 * @brief Warning logging
 * 
 * Ex. WARNING("function {} returns {}", functionName, functionReturnInt);
 */
#define WARNING(fmt, ...) \
    ::vox::log_warning(FMT_STRING(fmt) __VA_OPT__(, ) __VA_ARGS__)
/**
 * @brief Error logging
 * 
 * After logging error message, the program will be aborted.
 * Ex. ERROR("function {} returns {}", functionName, functionReturnInt);
 */
#define ERROR(fmt, ...) \
    ::vox::log_error(FMT_STRING(fmt) __VA_OPT__(, ) __VA_ARGS__)

/// VERBOSE with file and line information
#define VERBOSE_WITH_LOCATION(fmt, ...) \
    VERBOSE(fmt " [{}:{}]" __VA_OPT__(, ) __VA_ARGS__, __FILE__, __LINE__)
/// INFO with file and line information
#define INFO_WITH_LOCATION(fmt, ...) \
    INFO(fmt " [{}:{}]" __VA_OPT__(, ) __VA_ARGS__, __FILE__, __LINE__)
/// WARNING with file and line information
#define WARNING_WITH_LOCATION(fmt, ...) \
    WARNING(fmt " [{}:{}]" __VA_OPT__(, ) __VA_ARGS__, __FILE__, __LINE__)
/// ERROR with file and line information
#define ERROR_WITH_LOCATION(fmt, ...) \
    ERROR(fmt " [{}:{}]" __VA_OPT__(, ) __VA_ARGS__, __FILE__, __LINE__)

#define NOT_IMPLEMENTED() \
    ERROR_WITH_LOCATION("Not implemented.")

#define ASSERT(x, fmtValue, ...)                      \
    do {                                              \
        if (!(x)) [[unlikely]] {                      \
            auto msg = fmt::format(                   \
                fmtValue __VA_OPT__(, ) __VA_ARGS__); \
            ERROR_WITH_LOCATION(                      \
                "Assertion '{}' failed: {}",          \
                #x, msg);                             \
        }                                             \
    } while (false);
