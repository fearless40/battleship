#pragma once

#include "soa.hpp"
#include "soamemoryprotocol.hpp"
#include <compare>
#include <vector>

namespace term {

class Compositor {
public: // Types
  class Handle {
  private:
    unsigned int external_value{0};
    unsigned int map_index{0};

  public:
    std::strong_ordering operator<=>(const Handle &other) const = default;
    friend Compositor;
  };

  struct Rect {
    int x, y;
    int w, h;
  };

private:
  struct AABB {
    int x, y;
    int x2, y2;

    operator Rect() { return {x, y, x2 - x, y2 - y}; }
  };

  template <typename Tag> struct Range1D {
    int x, x2;
    std::strong_ordering operator<=>(const Range1D<Tag> &other) const = default;
    friend constexpr bool intersects(const Range1D<Tag> &r1,
                                     const Range1D<Tag> &r2) {
      return r1.x >= r2.x && r1.x <= r2.x2 || r1.x2 >= r2.x && r1.x2 <= r2.x2;
    };

    constexpr bool contains(int value) const noexcept {
      return value >= x && value <= x2;
    };

    struct X {};
    struct Y {};

    using XPos = Range1D<X>;
    using YPos = Range1D<Y>;
    using ZOrder = int;

    bool composition_dirty_{false};
    std::vector<Handle> dirty_layers_;
    std::vector<Handle> visible_layers_;
    std::vector<Handle> free_layers_;

    using SOA = util::soa::SOA<util::soa::memory_layout::DynamicArray, XPos,
                               YPos, ZOrder, Handle>;
    SOA soa;

  public:
    Handle get_base_layer() const { return {}; };
    Handle new_layer(Rect position, int zOrder);
    void layer_dirty(Handle layer);
    void layer_is_transperant(Handle layer);
    void layer_is_opaque(Handle layer);
    void hide(Handle layer);
    void show(Handle layer);
    void move(Handle layer, const Rect &position);
    void move(Handle layer, int zOrder);
    void move(Handle layer, const Rect &position, int zOrder);
  };
} // namespace term
