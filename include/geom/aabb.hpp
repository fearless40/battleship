#pragma once

namespace geom {

// X and Y are seperate to allow for strong types to be used

template <typename X, typename Y> struct AABB {
  X x, x2;
  Y y, y2;

  constexpr bool contains_point(X xp, Y yp) const noexcept {
    return (xp >= x && xp <= x2 && yp >= y && yp <= y2);
  }
};

} // namespace geom
