// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <ofxCore.h>
#include "ofxStatusStrings.h"

#include <sstream>
#include <stdexcept>
#include <string>

namespace openfx {

/**
 * @brief Base exception class for the OpenFX API wrapper
 */
class OfxException : public std::runtime_error {
 public:
  /**
   * @brief Construct a new API exception
   * @param code Error code from the C API
   * @param message Error message
   */
  OfxException(OfxStatus status, const std::string &msg)
      : std::runtime_error(createMessage(msg, status)), error_code_(status) {}

  /**
   * @brief Get the error code
   * @return The error code from the C API
   */
  int code() const noexcept { return error_code_; }

 private:
  OfxStatus error_code_;
  static std::string createMessage(const std::string &msg, OfxStatus status) {
    std::ostringstream oss;
    oss << "OpenFX error: " << msg << ": " << ofxStatusToString(status) << "\n";
    return oss.str();
  }
};

/**
 * @brief Exception thrown when a required property isn't found
 */
class PropertyNotFoundException : public OfxException {
 public:
  explicit PropertyNotFoundException(int code, const std::string &msg = "")
      : OfxException(code, msg) {}
};

/**
 * @brief Exception thrown when an clip isn't found
 */
class ClipNotFoundException : public OfxException {
 public:
  explicit ClipNotFoundException(int code, const std::string &msg = "") : OfxException(code, msg) {}
};

/**
 * @brief Exception thrown when an image isn't found
 */
class ImageNotFoundException : public OfxException {
 public:
  explicit ImageNotFoundException(int code, const std::string &msg = "")
      : OfxException(code, msg) {}
};

}  // namespace openfx
