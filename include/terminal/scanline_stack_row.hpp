#pragma once
#include "compositor_shared.hpp"
#include "stackhelpers.hpp"
#include <stack>
#include <vector>

namespace term::compositor::stack {
using namespace term::compositor::soa;
using VectorIT = std::vector<IteratorSOA>;
using RowStack = std::stack<IteratorSOA, std::vector<IteratorSOA>>;

using CRR = term::compositor::CRR;

struct RowCache {
  VectorIT &sorted_line;
  std::stack<IteratorSOA, std::vector<IteratorSOA>> &stack;
};

struct Row : public RowCache {
  X x_{0};
  X x2_{0};
  X x_max_{80};
  VectorIT::iterator next_;

  constexpr CRR line_next() {
    using namespace soa;

    auto handle_return = handle(stack.top());
    if (x_ < x(stack.top())) {
      x_ = x(stack.top());
    } else {
      x_ = x2_;
    }

    const auto end = sorted_line.end();
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

    return {handle_return, x_, x2_};
  }

  constexpr Row(RowCache &cache, int x_max) : RowCache(cache), x_max_(x_max) {};

  struct Sentinal {
    X x_max;
  };

  struct Iterator {
    Row &row;
    CRR last;
    CRR &operator*() { return last; }
    Iterator &operator++() {
      if (row.stack.empty()) {
        last.xEnd = row.x_max_;
      } else {
        last = row.line_next();
      }
      return *this;
    }
    bool operator!=(const Sentinal &sentinal) {
      return last.xEnd < sentinal.x_max;
    }
    // bool operator==(const Sentinal &sentinal) {
    //   return last.xEnd >= sentinal.x_max;
    // }
  };

  constexpr auto begin() {
    using namespace soa;
    util::stack::clear(stack);
    std::sort(sorted_line.begin(), sorted_line.end(), [](auto &l, auto &r) {
      unsigned long l_sort =
          ((x(l).underlying() & 0xFFFF) << 16) | (zorder(l) & 0xFFFF);
      unsigned long r_sort =
          ((x(r).underlying() & 0xFFFF) << 16) | (zorder(r) & 0xFFFF);
      return l_sort < r_sort;
    });
    next_ = sorted_line.begin() + 1;
    stack.push(sorted_line.front());

    // Run the iterator once to prime it...
    auto ret = Iterator(*this);
    ++ret;

    return ret;
  }
  constexpr auto end() { return Sentinal{x_max_}; }
};
}; // namespace term::compositor::stack
