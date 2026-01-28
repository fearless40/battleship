
#pragma once
#include "compositor_shared.hpp"
#include "stackhelpers.hpp"
#include <print>
#include <stack>
#include <vector>
namespace term::compositor {

class ScanLineRender {
  using IteratorSOA = typename SOA::Iterator;

  SOA &soa;
  int min_x_{0};
  int min_y_{0};
  int max_x_{80};
  int max_y_{80};
  int x_{0};
  int x2_{0};
  int y_{0};
  using VectorIT = std::vector<IteratorSOA>;
  VectorIT sorted_line;
  std::stack<IteratorSOA, std::vector<IteratorSOA>> stack{};
  VectorIT::iterator next_;

public:
  ScanLineRender(SOA &soa_to_use, int max_x, int max_y)
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

  struct ColSentinal {
    int x_max;
  };

  struct ColIterator {
    ScanLineRender &render;

    CRR last;
    CRR operator*() { return last; }
    auto operator++() {
      last = render.line_next();
      return this;
    }
    bool operator!=(const ColSentinal &sentinal) {
      return last.xEnd < sentinal.x_max;
    }
    bool operator==(const ColSentinal &sentinal) {
      return last.xEnd >= sentinal.x_max;
    }
  };
  constexpr auto begin_col() { return ColIterator{*this}; };
  constexpr auto end_col() { return ColSentinal{max_x_}; };

  struct RowSentinal {
    int y_max;
  };

  struct RowIterator {
    ScanLineRender &render_;
    int y_{};
    bool operator!=(const RowSentinal &sentinal) { return y_ < sentinal.y_max; }
    RowIterator &operator*() { return *this; }

    RowIterator &operator++() {
      ++y_;
      return *this;
    };

    constexpr auto begin() {
      render_.init_line();

      return render_.begin_col();
    }

    constexpr auto end() { return render_.end_col(); }
  };

  constexpr auto begin() { return RowIterator{*this, min_y_}; };
  constexpr auto end() { return RowSentinal{}; }

  friend struct RowIterator;

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
} // namespace term::compositor
