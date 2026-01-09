#pragma once
#include "compositor_shared.hpp"
#include <assert.h>
#include <print>
#include <source_location>
#include <string_view>

void log(const std::string_view msg,
         const std::source_location loc = std::source_location::current()) {
  // std::println("Fn: {} Line: {} Msg: {}", loc.function_name(), loc.line(),
  // msg);
}

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
  std::vector<soa::IteratorSOA> line_buffer_;

public:
  ScanLinePainter(SOA &soa_to_use, int max_x, int max_y)
      : soa(soa_to_use), max_x_(max_x), max_y_(max_y), output_(max_x_) {};

  // Returns the image and number of pixles it represents
  std::tuple<Handle, int, int> line_next() {
    log("Start");
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

    log("End");

    return {start->handle, x1, x2};
  }

private:
  void fill_current_line(int y) {
    log("Start");
    line_buffer_.clear();
    for (auto it = soa.begin(); it != soa.end(); ++it) {
      if (soa::gety(it).contains(y)) {
        line_buffer_.push_back(it);
      };
    }
    log("end");
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
    log("start");
    std::fill(output_.begin(), output_.end(), buffer{Handle{0, 0}, 0});
    log("end");
  }

  void render(std::vector<soa::IteratorSOA> &line) {

    log("start");
    for (auto &rect : line) {
      auto start = output_.begin() + soa::x(rect);
      auto end = output_.begin() + soa::x2(rect);
      if (end > output_.end())
        end = output_.end();
      std::replace_if(
          start, end, [&](auto &buf) { return buf.order < soa::zorder(rect); },
          buffer{soa::handle(rect), soa::zorder(rect)});
    }
    log("end");
  }

public:
  void init_line() {
    log("start");
    x_ = 0;
    x2_ = 0;

    clear_output();

    log("before fill");
    fill_current_line(y_);
    log("after fill");
    render(line_buffer_);
    next_ = output_.begin();
    line_finished = false;

    log("end");
  }
};
} // namespace term::compositor
