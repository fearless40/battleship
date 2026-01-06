#pragma once

#include "compositor_shared.hpp"

namespace term::compositor {

class ScanLinePainter {

  SOA &soa;
  int max_x_{80};
  int max_y_{80};
  int x_{0};
  int x2_{0};
  int y_{0};
  bool line_finished{false};

  struct buffer {
    Handle handle;
    ZOrder order;

    bool operator!=(const buffer &other) { return handle != other.handle; }
  };

  std::vector<buffer> output_;
  std::vector<buffer>::iterator next_;

public:
  ScanLinePainter(SOA &soa_to_use, int max_x, int max_y)
      : soa(soa_to_use), max_x_(max_x), max_y_(max_y) {
    output_.reserve(max_x_);
  };

  // Returns the image and number of pixles it represents
  std::tuple<Handle, int, int> line_next() {
    if (line_finished)
      return {compositor::Handle{0, 0}, 0, 0};

    const auto start = next_;
    const auto end = output_.end();

    while (next_ != end) {
      if (*next_ != *start) {
        break;
      }
      ++next_;
    }

    const auto x1 = std::distance(output_.begin(), start);

    const auto x2 = std::distance(output_.begin(), next_);

    if (x2 > max_x_) {
      ++y_;
      line_finished = true;
    }

    return {start->handle, x1, x2};
  }

private:
  std::vector<soa::IteratorSOA> fill_current_line(int y) {

    std::vector<soa::IteratorSOA> ret;
    for (auto it = soa.begin(); it != soa.end(); ++it) {
      if (soa::gety(it).contains(y)) {
        ret.push_back(it);
      };
    }
    // std::println("Finsihed filling sorted line number entrires: {}",
    //              sorted_line.size());
  }

  // void sort_current_line() {
  //   std::println("Sorting");
  //   std::sort(sorted_line.begin(), sorted_line.end(), [](auto &l, auto &r) {
  //     unsigned long l_sort = ((x(l) & 0xFFFF) << 16) | (zorder(l) & 0xFFFF);
  //     unsigned long r_sort = ((x(r) & 0xFFFF) << 16) | (zorder(r) & 0xFFFF);
  //     return l_sort < r_sort;
  //   });
  //   std::println("Done sorting");
  // }

  void clear_output() {

    std::fill(output_.begin(), output_.end(), buffer{Handle{0, 0}, 0});
  }

  void render(std::vector<soa::IteratorSOA> &line) {
    for (auto &rect : line) {
      auto start = output_.begin() + soa::x(rect);
      auto end = output_.begin() + soa::x2(rect);
      std::replace_if(
          start, end, [&](auto &buf) { return buf.order < soa::zorder(rect); },
          buffer{soa::handle(rect), soa::zorder(rect)});
    }
  }

public:
  void init_line() {
    x_ = 0;
    x2_ = 0;

    clear_output();

    auto line = fill_current_line(y_);
    render(line);
    next_ = output_.begin();
    line_finished = false;
  }
};
} // namespace term::compositor
