// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <ofxCore.h>
#include <ofxExceptions.h>

namespace openfx {

// Generic conversion function for any container to OfxRectI
// Works with std::array, std::vector, C-style arrays, and any container supporting operator[]
template <typename Container>
OfxRectI toOfxRectI(const Container& container) {
  // Check container size at runtime for dynamic containers
  if (container.size() < 4) {
    throw OfxException(kOfxStatErrValue,
                       "Container has fewer than 4 elements required for OfxRectI");
  }

  OfxRectI rect;
  rect.x1 = container[0];
  rect.y1 = container[1];
  rect.x2 = container[2];
  rect.y2 = container[3];

  return rect;
}

// Specialization for fixed-size arrays where we can use static_assert
template <typename T, size_t N>
OfxRectI toOfxRectI(const std::array<T, N>& arr) {
  static_assert(N >= 4, "Array must have at least 4 elements to convert to OfxRectI");

  OfxRectI rect;
  rect.x1 = arr[0];
  rect.y1 = arr[1];
  rect.x2 = arr[2];
  rect.y2 = arr[3];

  return rect;
}

}  // namespace openfx
