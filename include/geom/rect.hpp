#pragma once

#include "compositor_shared.hpp"
#include "onedrange.hpp"
#include <compare>
namespace geom {

template <typename X, typename Y, typename W, typename H> struct RectSafe {
  X x;
  Y y;
  W w;
  H h;
};

template <typename XDimension, typename YDimension> struct TypedRect {
  using X_t = typename XDimension::position_t;
  using Y_t = typename YDimension::position_t;
  using Width_t = XDimension;
  using Height_t = YDimension;

  X_t x;
  Y_t y;
  Width_t w;
  Height_t h;

  constexpr TypedRect(X_t x_, Y_t y_, Width_t w_, Height_t h_)
      : x{x_}, y{y_}, w{w_}, h{h_} {}

  constexpr TypedRect(Width_t w_, Height_t h_) : x{0}, y{0}, w{w_}, h{h_} {}

  constexpr TypedRect(X_t x1_, X_t x2_, Y_t y1_, Y_t y2_)
      : x{x1_}, y{y1_}, w{x1_, x2_}, h{y1_, y2_} {}

  constexpr Range1D<Width_t> xRange() const noexcept { return Range1D{x, w}; }
  constexpr Range1D<Height_t> yRange() const noexcept { return Range1D{y, h}; }
};

using IntRect = RectSafe<int, int, int, int>;

} // namespace geom
