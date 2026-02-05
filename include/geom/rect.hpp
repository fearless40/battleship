#pragma once

namespace util {

template <typename X, typename Y, typename W, typename H> struct RectSafe {
  X x;
  Y y;
  W w;
  H h;
};

using IntRect = RectSafe<int, int, int, int>;

} // namespace util
