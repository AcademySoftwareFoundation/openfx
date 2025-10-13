// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <chrono>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

namespace openfx {

// String formatter: forward decls
template <typename... Args>
std::string format(const std::string& fmt, Args&&... args);

inline void format_impl(std::ostringstream& oss, const char* fmt);

template <typename T, typename... Args>
void format_impl(std::ostringstream& oss, const char* fmt, T&& value, Args&&... args);

/**
 * @brief Simple, customizable logging facility for the OpenFX API wrapper
 */
class Logger {
 public:
  /**
   * @brief Log levels
   */
  enum class Level {
    Info,     ///< Informational messages
    Warning,  ///< Warning messages
    Error     ///< Error messages
  };

  /**
   * @brief Log handler function type
   *
   * @param level The log level
   * @param timestamp Timestamp when log was created
   * @param message The log message
   */
  using LogHandler = std::function<void(
      Level level, std::chrono::system_clock::time_point timestamp, const std::string& message)>;

  /**
   * @brief Set a custom log handler
   *
   * @param handler The log handler function
   */
  static void setLogHandler(LogHandler handler);

  /**
   * @brief Log an informational message
   *
   * @param message The message to log
   */
  static void info(const std::string& message);

  /**
   * @brief Log an informational message with formatting
   *
   * @param format Format string with {} placeholders
   * @param args Arguments to format
   */
  template <typename... Args>
  static void info(const std::string& format, Args&&... args) {
    log(Level::Info, format_message(format, std::forward<Args>(args)...));
  }

  /**
   * @brief Log a warning message
   *
   * @param message The message to log
   */
  static void warn(const std::string& message);

  /**
   * @brief Log a warning message with formatting
   *
   * @param format Format string with {} placeholders
   * @param args Arguments to format
   */
  template <typename... Args>
  static void warn(const std::string& format, Args&&... args) {
    log(Level::Warning, format_message(format, std::forward<Args>(args)...));
  }

  /**
   * @brief Log an error message
   *
   * @param message The message to log
   */
  static void error(const std::string& message);

  /**
   * @brief Log an error message with formatting
   *
   * @param format Format string with {} placeholders
   * @param args Arguments to format
   */
  template <typename... Args>
  static void error(const std::string& format, Args&&... args) {
    log(Level::Error, format_message(format, std::forward<Args>(args)...));
  }

 private:
  /**
   * @brief Log a message with the specified level
   *
   * @param level The log level
   * @param message The message to log
   */
  static void log(Level level, const std::string& message);

  /**
   * @brief Default log handler implementation
   *
   * @param level The log level
   * @param timestamp Timestamp when log was created
   * @param message The message to log
   */
  static void defaultLogHandler(Level level, std::chrono::system_clock::time_point timestamp,
                                const std::string& message);

  /**
   * @brief Format a message using the simple formatter
   *
   * @param format Format string with {} placeholders
   * @param args Arguments to format
   * @return Formatted string
   */
  template <typename... Args>
  static std::string format_message(const std::string& format, Args&&... args) {
    if constexpr (sizeof...(args) == 0) {
      return format;
    } else {
      return ::openfx::format(format, std::forward<Args>(args)...);
    }
  }

  // Static member variables (inline in C++17)
  static inline LogHandler g_logHandler = defaultLogHandler;
  static inline std::mutex g_logMutex;
};

// Inline implementations for non-template methods

inline void Logger::setLogHandler(LogHandler handler) {
  if (handler) {
    std::lock_guard<std::mutex> lock(g_logMutex);
    g_logHandler = std::move(handler);
  } else {
    std::lock_guard<std::mutex> lock(g_logMutex);
    g_logHandler = defaultLogHandler;
  }
}

inline void Logger::info(const std::string& message) { log(Level::Info, message); }

inline void Logger::warn(const std::string& message) { log(Level::Warning, message); }

inline void Logger::error(const std::string& message) { log(Level::Error, message); }

inline void Logger::log(Level level, const std::string& message) {
  auto timestamp = std::chrono::system_clock::now();

  std::lock_guard<std::mutex> lock(g_logMutex);
  g_logHandler(level, timestamp, message);
}

inline void Logger::defaultLogHandler(Level level, std::chrono::system_clock::time_point timestamp,
                                      const std::string& message) {
  // Convert timestamp to local time
  std::time_t time = std::chrono::system_clock::to_time_t(timestamp);
  std::tm local_time;
#ifdef _WIN32
    // Microsoft’s localtime_s expects arguments in reverse order compared to C11's localtime_s:
    localtime_s(&local_time, &time);
#else
    // POSIX
    localtime_r(&time, &local_time);
#endif

  // Stream to write log message
  std::ostream& os = (level == Level::Error) ? std::cerr : std::cout;

  // Level prefix
  const char* levelStr = "";
  switch (level) {
    case Level::Info:
      levelStr = "INFO";
      break;
    case Level::Warning:
      levelStr = "WARN";
      break;
    case Level::Error:
      levelStr = "ERROR";
      break;
  }

  // Write formatted log message
  os << "[" << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S") << "][" << levelStr << "] " << message
     << std::endl;
}

/**
 * @brief String formatter with {} placeholders (similar to std::format)
 *
 * @param fmt Format string with {} placeholders
 * @param args Arguments to format
 * @return Formatted string
 */
template <typename... Args>
std::string format(const std::string& fmt, Args&&... args) {
  std::ostringstream oss;
  format_impl(oss, fmt.c_str(), std::forward<Args>(args)...);
  return oss.str();
}

/**
 * @brief Implementation detail for the string formatter
 *
 * @param oss Output string stream
 * @param fmt Format string
 */
inline void format_impl(std::ostringstream& oss, const char* fmt) { oss << fmt; }

/**
 * @brief Implementation detail for the string formatter
 *
 * @param oss Output string stream
 * @param fmt Format string
 * @param value Value to format
 * @param args Remaining arguments
 */
template <typename T, typename... Args>
void format_impl(std::ostringstream& oss, const char* fmt, T&& value, Args&&... args) {
  const char* pos = std::strchr(fmt, '{');

  // Check if we have a valid placeholder with closing }
  if (pos && pos[1] == '}') {
    // Write everything up to the placeholder
    oss.write(fmt, pos - fmt);
    // Write the value
    oss << value;
    // Continue with the rest of the format string
    format_impl(oss, pos + 2, std::forward<Args>(args)...);
  } else {
    // No valid placeholder, just write the rest of the format string
    oss << fmt;
  }
}

}  // namespace openfx
