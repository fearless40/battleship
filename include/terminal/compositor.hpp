#pragma once

#include "soa.hpp"
#include "soamemorylayout.hpp"
#include <algorithm>
#include <compare>
#include <iterator>
#include <print>
#include <stack>
#include <vector>

namespace term {

class Compositor {
public: // Types
  class Handle {
  private:
    unsigned int external_value{0};
    unsigned int map_index{0};

  public:
    unsigned int index() { return external_value; }
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
  };

  struct X {};
  struct Y {};

  using XPos = Range1D<X>;
  using YPos = Range1D<Y>;
  using ZOrder = int;

  bool composition_dirty_{false};

  // These vectors are packed (no holes)
  std::vector<Handle> dirty_layers_;
  std::vector<Handle> visible_layers_;

  using SOA = util::soa::SOA<util::soa::memory_layout::DynamicArray, XPos, YPos,
                             ZOrder, Handle>;
  SOA soa;

public:
  friend class ScanLineIterator;
  class ScanLineIterator {
    using IteratorSOA = typename SOA::Iterator;

    SOA &soa;
    int max_x_{80};
    int max_y_{80};
    int x_{0};
    int x2_{0};
    int y_{0};
    using VectorIT = std::vector<IteratorSOA>;
    VectorIT sorted_line;
    std::stack<IteratorSOA, std::vector<IteratorSOA>> stack{};
    VectorIT::iterator next_;

    static constexpr auto getx(IteratorSOA &it) {
      return it.template get<XPos>();
    };
    static constexpr auto gety(IteratorSOA &it) {
      return it.template get<YPos>();
    };
    static constexpr auto x(IteratorSOA &it) {
      return it.template get<XPos>().x;
    };
    static constexpr auto x2(IteratorSOA &it) {
      return it.template get<XPos>().x2;
    };
    static constexpr auto zorder(IteratorSOA &it) {
      return it.template get<ZOrder>();
    };
    static constexpr auto handle(IteratorSOA &it) {
      return it.template get<Handle>();
    }

  public:
    ScanLineIterator(SOA &soa_to_use, int max_x, int max_y)
        : soa(soa_to_use), max_x_(max_x), max_y_(max_y) {};

    // Returns the image and number of pixles it represents
    std::tuple<Handle, int, int> line_next() {
      auto handle_return = handle(stack.top());
      if (x_ < x(stack.top())) {
        x_ = x(stack.top());
      } else {
        x_ = x2_;
      }

      auto end = sorted_line.end();
      while (next_ != end and x2_ > x2(*next_)) {
        ++next_;
      }

      if ((next_ == end) or ((zorder(*next_) < zorder(stack.top())) and
                             (x2_ < x2(stack.top())))) {
        x2_ = x2(stack.top()) + 1;
      } else {
        x2_ = x(*next_);
        if (next_ != end) {
          stack.push(*next_);
          ++next_;
        }
      }

      std::println("[{},{}] Stack [{},{}]", x_, x2_, x(stack.top()),
                   x2(stack.top()));

      while (stack.size() > 0 and x2(stack.top()) < x2_) {
        stack.pop();
      }

      return {handle_return, x_, x2_};
    }

  private:
    void fill_current_line(int y) {

      std::println("Filling sorted_line");
      sorted_line.clear();
      for (auto it = soa.begin(); it != soa.end(); ++it) {
        if (gety(it).contains(y)) {
          sorted_line.push_back(it);
        };
      }
      std::println("Finsihed filling sorted line number entrires: {}",
                   sorted_line.size());
    }

    void sort_current_line() {
      std::println("Sorting");
      std::sort(sorted_line.begin(), sorted_line.end(), [](auto &l, auto &r) {
        unsigned long l_sort = ((x(l) & 0xFFFF) << 16) | (zorder(l) & 0xFFFF);
        unsigned long r_sort = ((x(r) & 0xFFFF) << 16) | (zorder(r) & 0xFFFF);
        return l_sort < r_sort;
      });
      std::println("Done sorting");
    }

    void clear_stack() {
      std::println("Clearing stack");

      while (stack.size() > 0)
        stack.pop();
      std::println("Done CLearing stack");
    }

  public:
    void init_line() {
      x_ = 0;
      x2_ = 0;
      clear_stack();

      fill_current_line(y_);
      sort_current_line();
      next_ = sorted_line.begin() + 1;
      stack.push(*sorted_line.begin());
    }
  };

private:
  int max_x_{80};
  int max_y_{80};
  Handle next_handle_{};

  Handle next_handle() {
    next_handle_.external_value += 1;
    next_handle_.map_index += 1;
    return next_handle_;
  }

  void new_layer_with_handle(Rect position, int zOrder, Handle handle) {
    soa.push_back({position.x, position.w + position.x},
                  {position.y, position.h + position.y}, zOrder, handle);
  }

public:
  Compositor() { new_layer_with_handle({0, 0, max_x_, max_y_}, 0, {}); }

  Handle get_base_layer() const { return {}; };
  Handle new_layer(Rect position, int zOrder) {
    new_layer_with_handle(position, zOrder, next_handle());
    return next_handle_;
  };

  ScanLineIterator get_scanline_render() {
    return ScanLineIterator(soa, max_x_, max_y_);
  }

  // void layer_dirty(Handle layer);
  // void layer_is_transperant(Handle layer);
  // void layer_is_opaque(Handle layer);
  // void hide(Handle layer);
  // void show(Handle layer);
  // void move(Handle layer, const Rect &position);
  // void move(Handle layer, int zOrder);
  // void move(Handle layer, const Rect &position, int zOrder);

  // void flatten_as_scanlines();
  // void flatten_as_rects();
};
}; // namespace term
