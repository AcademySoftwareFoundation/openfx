// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <ofxCore.h>
#include "ofxExceptions.h"

#include <array>
#include <type_traits>

namespace openfx {

namespace detail {
// Element count traits
template <typename T>
inline constexpr size_t ofx_elem_count = 0;
template <>
inline constexpr size_t ofx_elem_count<OfxRectI> = 4;
template <>
inline constexpr size_t ofx_elem_count<OfxRectD> = 4;
template <>
inline constexpr size_t ofx_elem_count<OfxPointI> = 2;
template <>
inline constexpr size_t ofx_elem_count<OfxPointD> = 2;

// Generic conversion implementation
template <typename Result, typename Container>
Result toOfx(const Container& container) {
  if (container.size() < ofx_elem_count<Result>)
    throw OfxException(kOfxStatErrValue, "Container has too few elements");

  Result result;
  if constexpr (ofx_elem_count<Result> == 4) {
    result.x1 = container[0];
    result.y1 = container[1];
    result.x2 = container[2];
    result.y2 = container[3];
  } else {
    result.x = container[0];
    result.y = container[1];
  }
  return result;
}
}  // namespace detail

// Integer type functions - use std::remove_reference_t to handle references properly
template <typename Container>
auto toOfxRectI(const Container& c)
    -> std::enable_if_t<std::is_integral_v<std::remove_reference_t<decltype(c[0])>>, OfxRectI> {
  return detail::toOfx<OfxRectI>(c);
}

template <typename Container>
auto toOfxPointI(const Container& c)
    -> std::enable_if_t<std::is_integral_v<std::remove_reference_t<decltype(c[0])>>, OfxPointI> {
  return detail::toOfx<OfxPointI>(c);
}

// Floating-point type functions
template <typename Container>
auto toOfxRectD(const Container& c)
    -> std::enable_if_t<std::is_floating_point_v<std::remove_reference_t<decltype(c[0])>>,
                        OfxRectD> {
  return detail::toOfx<OfxRectD>(c);
}

template <typename Container>
auto toOfxPointD(const Container& c)
    -> std::enable_if_t<std::is_floating_point_v<std::remove_reference_t<decltype(c[0])>>,
                        OfxPointD> {
  return detail::toOfx<OfxPointD>(c);
}
}  // namespace openfx
