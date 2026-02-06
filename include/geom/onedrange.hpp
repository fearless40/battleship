#pragma once
#include "relative_dimension.hpp"
#include "typed_position.hpp"
#include <compare>
#include <ranges>

namespace geom {

/***
 * Description: Range of values [n,m). Inclusive n, exclusive m
 */
template <typename DimensonT> struct Range1D {
  using value_t = typename DimensonT::position_t;

  value_t p, p2;
  std::strong_ordering operator<=>(const Range1D &other) const = default;
  friend constexpr bool intersects(const Range1D &r1, const Range1D &r2) {
    return r1.p >= r2.p && r1.p < r2.p2 || r1.p2 >= r2.p && r1.p2 <= r2.p2;
  };

  constexpr bool contains(const value_t &value) const noexcept {
    return value >= p && value < p2;
  };

  constexpr auto as_range() const noexcept { return std::views::iota(p, p2); };
};

} // namespace geom
