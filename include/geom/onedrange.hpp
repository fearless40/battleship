#pragma once
#include <compare>

namespace util {

/***
 * Description: Range of values [n,m). Inclusive n, exclusive m
 */
template <typename T, typename Tag> struct Range1D {
  T x, x2;
  std::strong_ordering
  operator<=>(const Range1D<T, Tag> &other) const = default;
  friend constexpr bool intersects(const Range1D<T, Tag> &r1,
                                   const Range1D<T, Tag> &r2) {
    return r1.x >= r2.x && r1.x <= r2.x2 || r1.x2 >= r2.x && r1.x2 <= r2.x2;
  };

  constexpr bool contains(T value) const noexcept {
    return value >= x && value < x2;
  };
};

} // namespace util
