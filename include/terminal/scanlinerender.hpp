
#pragma once
#include "compositor_shared.hpp"
#include <stack>

namespace term::compositor {
class ScanLineRender {
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
  ScanLineRender(SOA &soa_to_use, int max_x, int max_y)
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

    if ((next_ == end) or
        ((zorder(*next_) < zorder(stack.top())) and (x2_ < x2(stack.top())))) {
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
} // namespace term::compositor
