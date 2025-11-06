// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

/**
 * @file ofxSpan.h
 * @brief Provides std::span or compatible implementation for all C++ versions
 *
 * This header provides a span type (non-owning view over contiguous sequences)
 * that works with C++17 and later. In C++20+, it uses std::span. For earlier
 * versions, it uses tcbrindle/span from the Conan package tcb-span.
 *
 * The tcb-span package is automatically provided via Conan dependencies.
 * See https://github.com/tcbrindle/span for more information.
 *
 * Usage:
 *   openfx::span<const int> values = myVector;
 *   openfx::span<double> writable = myArray;
 *   for (auto v : values) { ... }
 */

// Use std::span if available (C++20+)
#if __cplusplus >= 202002L && __has_include(<span>)
  #include <span>
  namespace openfx {
    template<typename T, std::size_t Extent = std::dynamic_extent>
    using span = std::span<T, Extent>;

    inline constexpr std::size_t dynamic_extent = std::dynamic_extent;
  }
  #define OPENFX_HAS_STD_SPAN 1

// Otherwise use tcbrindle/span for C++11/14/17 (via Conan package tcb-span)
#else
  // The tcb-span Conan package provides tcb/span.hpp
  // You can override this by defining OPENFX_SPAN_HEADER before including this file:
  // #define OPENFX_SPAN_HEADER "my_span.hpp"

  #ifndef OPENFX_SPAN_HEADER
    #define OPENFX_SPAN_HEADER <tcb/span.hpp>
  #endif

  // Configure tcbrindle/span to use openfx namespace
  #define TCB_SPAN_NAMESPACE_NAME openfx
  #include OPENFX_SPAN_HEADER
  #undef TCB_SPAN_NAMESPACE_NAME

  // Note: tcbrindle/span already defines openfx::span, openfx::dynamic_extent,
  // and openfx::make_span helpers
  #define OPENFX_HAS_STD_SPAN 0
#endif

// Only define make_span helpers when using std::span (C++20+)
// tcbrindle/span already provides these in the openfx namespace
#if OPENFX_HAS_STD_SPAN
namespace openfx {

/**
 * @brief Helper to create a span from a C-style array with size
 *
 * Usage:
 *   const char* items[] = {"a", "b", "c"};
 *   auto s = make_span(items, 3);
 */
template<typename T>
constexpr span<T> make_span(T* ptr, std::size_t count) noexcept {
  return span<T>(ptr, count);
}

/**
 * @brief Helper to create a span from a C-style array
 *
 * Usage:
 *   const char* items[] = {"a", "b", "c"};
 *   auto s = make_span(items);  // Size deduced as 3
 */
template<typename T, std::size_t N>
constexpr span<T, N> make_span(T (&arr)[N]) noexcept {
  return span<T, N>(arr);
}

/**
 * @brief Helper to create a span from a container (vector, array, etc.)
 *
 * Usage:
 *   std::vector<int> v = {1, 2, 3};
 *   auto s = make_span(v);
 */
template<typename Container>
constexpr auto make_span(Container& cont) noexcept
    -> span<typename Container::value_type> {
  return span<typename Container::value_type>(cont);
}

template<typename Container>
constexpr auto make_span(const Container& cont) noexcept
    -> span<const typename Container::value_type> {
  return span<const typename Container::value_type>(cont);
}

}  // namespace openfx
#endif  // OPENFX_HAS_STD_SPAN

/**
 * Usage Examples:
 *
 * // From vector:
 * std::vector<int> vec = {1, 2, 3};
 * openfx::span<const int> s1 = vec;
 *
 * // From array:
 * std::array<double, 4> arr = {1.0, 2.0, 3.0, 4.0};
 * openfx::span<double> s2 = arr;
 *
 * // From C-style array:
 * const char* items[] = {"a", "b", "c"};
 * openfx::span<const char*> s3 = openfx::make_span(items, 3);
 *
 * // Range-for:
 * for (auto item : s3) {
 *   // process item
 * }
 *
 * // Bounds-checked access (debug mode with tcbrindle/span):
 * auto first = s1[0];      // OK
 * auto oob = s1[100];      // Throws/asserts in debug mode
 *
 * // Named accessors:
 * auto front = s1.front();
 * auto back = s1.back();
 * bool empty = s1.empty();
 * size_t size = s1.size();
 *
 * // Subspans:
 * auto first_two = s1.subspan(0, 2);
 * auto last_two = s1.subspan(s1.size() - 2);
 */
