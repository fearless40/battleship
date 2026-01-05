#pragma once

#include "aabb.hpp"
#include <iterator>

namespace collision {
using AABB = util::AABB<int, int>;

constexpr bool aabb_collision(const AABB &pos1, const AABB &pos2) noexcept {

  if (pos1.x <= pos2.x2 && pos1.x2 >= pos2.x && pos1.y <= pos2.y2 &&
      pos1.y2 >= pos2.y) {
    return true;
  }
  return false;
}

constexpr bool any_collision(auto &&range, auto projection) {
  auto end = std::end(range);
  for (auto first = std::begin(range); first != end; ++first) {
    for (auto second = first + 1; second != end; ++second) {
      if (aabb_collision(projection(*first), projection(*second)))
        return true;
    }
  }
  return false;
}
} // namespace collision
/*using CollisionIndex = std::pair<size_t, size_t>;
constexpr std::vector<CollisionIndex> any_collision(auto &&range, auto
projection)
{
    auto end = std::end(range);
    for (auto first = std::begin(range); first != end; ++first)
    {
        for (auto second = first + 1; second != end; ++second)
        {
            if (aabb_collision(projection(*first), projection(*second)))
                return true;
        }
    }
    return false;
}*/
