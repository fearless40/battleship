
#pragma once
#include "aabb.hpp"
#include "compositor_shared.hpp"
#include "scanline_stack_row.hpp"
#include "stackhelpers.hpp"
#include <stack>
#include <vector>
namespace term::compositor::stack {

class Render {
  util::AABB<int, int> extents;
  RowStack cache_stack;
  VectorIT cache_row;
  term::compositor::SOA &compositor_data;

  static constexpr VectorIT &get_surfaces_in_row(VectorIT &input, SOA &soa,
                                                 int y) {
    using namespace soa;
    input.clear();
    for (auto it = soa.begin(); it != soa.end(); ++it) {
      if (gety(it).contains(y)) {
        input.push_back(it);
      };
    }
    return input;
  }

  static constexpr void sort_surfaces(VectorIT &input) {
    using namespace soa;
    std::sort(input.begin(), input.end(), [](auto &l, auto &r) {
      unsigned long l_sort = ((x(l) & 0xFFFF) << 16) | (zorder(l) & 0xFFFF);
      unsigned long r_sort = ((x(r) & 0xFFFF) << 16) | (zorder(r) & 0xFFFF);
      return l_sort < r_sort;
    });
  }

public:
  Render(term::compositor::SOA &soa_input, util::AABB<int, int> &&extents_)
      : compositor_data(soa_input), extents{extents_} {}

  struct Iterator;
  friend struct Iterator;

  struct Sentinal {
    int y_max;
  };

  struct Iterator {
    term::compositor::SOA &soa;
    RowCache cache;
    int x_max_;
    int y_;

    Iterator() = delete;
    Iterator(Iterator &) = delete;
    Iterator(Iterator &&) = default;

    constexpr Iterator(term::compositor::SOA &soa_input, RowCache &&cache_input,
                       util::AABB<int, int> extents_)
        : soa(soa_input), cache(cache_input), y_(extents_.y),
          x_max_(extents_.x2) {}

    bool operator!=(const Sentinal &sentinal) { return y_ < sentinal.y_max; }
    Row operator*() {

      using namespace soa;
      Render::get_surfaces_in_row(cache.sorted_line, soa, y_);
      Render::sort_surfaces(cache.sorted_line);

      return Row(cache, x_max_);
    }

    Iterator &operator++() {
      ++y_;
      return *this;
    };
  };

  constexpr auto begin() {
    return Iterator{compositor_data, RowCache{cache_row, cache_stack}, extents};
  }
  constexpr auto end() { return Sentinal{extents.y2 + 1}; }
};

class StackRender {

  using VectorIT = std::vector<soa::IteratorSOA>;
  using RowStack = std::stack<soa::IteratorSOA, std::vector<soa::IteratorSOA>>;

  SOA &soa;
  int min_x_{0};
  int min_y_{0};
  int max_x_{80};
  int max_y_{80};
  int x_{0};
  int x2_{0};
  int y_{0};
  VectorIT sorted_line;
  std::stack<IteratorSOA, std::vector<IteratorSOA>> stack{};
  VectorIT::iterator next_;

public:
  StackRender(SOA &soa_to_use, int max_x, int max_y)
      : soa(soa_to_use), max_x_(max_x), max_y_(max_y) {};

  // Returns the image and number of pixles it represents
  CRR line_next() {
    using namespace soa;
    auto handle_return = handle(stack.top());
    if (x_ < x(stack.top())) {
      x_ = x(stack.top());
    } else {
      x_ = x2_;
    }

    auto end = sorted_line.end();
    while (next_ != end and x2_ >= x2(*next_)) {
      ++next_;
    }

    if ((next_ == end) or
        ((zorder(*next_) < zorder(stack.top())) and (x2_ < x2(stack.top())))) {
      x2_ = x2(stack.top()); //+ 1;
    } else {
      x2_ = x(*next_);
      if (next_ != end) {
        stack.push(*next_);
        ++next_;
      }
    }
    while (stack.size() > 0 and x2(stack.top()) <= x2_) {
      stack.pop();
    }

    // std::println("[{},{}] Stack Sz ", x_, x2_, stack.size());
    // print_stack(stack, [](IteratorSOA &s) {
    //   std::print(" [{},{},{}]", x(s), x2(s), zorder(s));
    // });
    // std::println();
    //
    if (x2_ > max_x_) {
      ++y_;
    }

    return {handle_return, x_, x2_};
  }

private:
  void fill_current_line(int y) {
    using namespace soa;
    sorted_line.clear();
    for (auto it = soa.begin(); it != soa.end(); ++it) {
      if (gety(it).contains(y)) {
        sorted_line.push_back(it);
      };
    }
  }

  void sort_current_line() {
    using namespace soa;
    std::sort(sorted_line.begin(), sorted_line.end(), [](auto &l, auto &r) {
      unsigned long l_sort = ((x(l) & 0xFFFF) << 16) | (zorder(l) & 0xFFFF);
      unsigned long r_sort = ((x(r) & 0xFFFF) << 16) | (zorder(r) & 0xFFFF);
      return l_sort < r_sort;
    });
  }

  void clear_stack() { util::stack::clear(stack); }

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
} // namespace term::compositor::stack
