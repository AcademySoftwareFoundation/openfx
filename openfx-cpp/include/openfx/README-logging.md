# OFX API Wrapper Logging System


## Overview

The OFX API Wrapper provides a lightweight, thread-safe customizable logging system. It supports very simple `fmt`-style formatting.

## Basic Usage

```cpp
#include "openfx/ofxLog.h"

// Log simple messages
openfx::Logger::info("Application started");
openfx::Logger::warn("Resource usage is high");
openfx::Logger::error("Operation failed");

// Log with formatting (similar to std::format)
openfx::Logger::info("Processing resource: {}", resource_id);
openfx::Logger::warn("Memory usage: {}MB", memory_usage);
openfx::Logger::error("Failed to process item {} in category {}", item_id, category);
```

## Log Levels

The logging system supports three log levels:

- **Info**: General information messages
- **Warning**: Warning messages that don't prevent operation
- **Error**: Error messages indicating failures

## Custom Log Handlers

You can customize where and how log messages are processed by providing your own log handler:

```cpp
// Create a custom log handler that writes to a file
std::shared_ptr<std::ofstream> logfile = 
    std::make_shared<std::ofstream>("application.log", std::ios::app);
    
openfx::Logger::setLogHandler(
    [logfile](openfx::Logger::Level level, 
             std::chrono::system_clock::time_point timestamp,
             const std::string& message) {
        // Convert timestamp to local time
        std::time_t time = std::chrono::system_clock::to_time_t(timestamp);
        std::tm local_tm = *std::localtime(&time);
        
        // Level to string
        const char* levelStr = "";
        switch (level) {
            case openfx::Logger::Level::Info:    levelStr = "INFO"; break;
            case openfx::Logger::Level::Warning: levelStr = "WARN"; break;
            case openfx::Logger::Level::Error:   levelStr = "ERROR"; break;
        }
        
        // Write to file
        *logfile << "["
                << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S")
                << "]["
                << levelStr
                << "] "
                << message
                << std::endl;
    }
);
```

## String Formatting

The logging system includes a simple string formatting utility that uses `{}` placeholders, similar to `std::format`:

```cpp
// Single placeholder
openfx::Logger::info("Processing file: {}", filename);

// Multiple placeholders 
openfx::Logger::info("Transfer completed: {} bytes in {} seconds", bytes, seconds);

// You can also use the formatter directly
std::string msg = openfx::format("User {} logged in from {}", username, ip_address);
```

## Default Log Format

The default log handler formats messages as:

```
[YYYY-MM-DD HH:MM:SS][LEVEL] Message
```

For example:
```
[2025-02-28 14:30:45][INFO] Application started
[2025-02-28 14:30:46][WARN] Memory usage above threshold: 85%
[2025-02-28 14:30:47][ERROR] Failed to connect to database
```

## Thread Safety

The logging system is thread-safe and can be used from multiple threads simultaneously. A mutex protects the log handler to ensure that log messages are not interleaved.

## Performance Considerations

- The logging system is designed to be lightweight, but frequent logging can impact performance
- When using custom log handlers, consider implementing buffering for high-volume logging
- String formatting occurs even if the log message is filtered out, so consider adding level checks for verbose logging:

```cpp
if (is_debug_mode) {
    openfx::Logger::info("Detailed debug info: {}", expensive_to_compute_string());
}
```

## Integration with Error Handling

The logging system is designed to work well with the OFX API Wrapper's exception system:

```cpp
try {
    // Some operation
} catch (const openfx::ApiException& e) {
    openfx::Logger::error("API error occurred: {} (code: {})", e.what(), e.code());
    // Handle the exception
}
```

-------------
Copyright OpenFX and contributors to the OpenFX project.

`SPDX-License-Identifier: BSD-3-Clause`

