#include "compositor.hpp"
#include "compositor_shared.hpp"
#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <iterator>
#include <print>
#include <ranges>

template <typename RENDER>
std::string convert_output_to_string(RENDER &render) {
  std::string output;
  const char *letters = "0abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int count = 0;
  for (int i = 0; i < 10; ++i) {
    auto [handle, x, x2] = render.line_next();
    for (int pos = x; pos < x2; ++pos) {
      output += letters[handle.index()];
    }
    // std::println("Count={}, Text={}", ++count, output);
  }

  return output;
}

TEST_CASE("ScanlineRenders 1 line only", "[scanlinerender]") {

  using namespace term;
  using Rect = term::compositor::Rect;
  using X = term::compositor::X;
  using Y = term::compositor::Y;
  using Width = term::compositor::Width;
  using Height = term::compositor::Height;
  Compositor comp{15, 1};

  const std::string expected = "0ab00cdec000000";

  comp.new_layer(Rect{X{1}, Y{0}, Width{1}, Height{1}}, 1);
  comp.new_layer(Rect{X{2}, Y{0}, Width{1}, Height{1}}, 2);
  comp.new_layer(Rect{X{5}, Y{0}, Width{4}, Height{1}}, 1);
  comp.new_layer(Rect{X{6}, Y{0}, Width{1}, Height{1}}, 3);
  comp.new_layer(Rect{X{6}, Y{0}, Width{2}, Height{1}}, 2);

  // SECTION("Stack based render") {
  //   auto render = comp.get_stack_render_old();
  //   render.init_line();
  //   REQUIRE(convert_output_to_string(render) == expected);
  // }
  //
  // SECTION("Painter based render") {
  //
  //   auto render = comp.get_painter_render();
  //   render.init_line();
  //   REQUIRE(convert_output_to_string(render) == expected);
  // }
  //
  SECTION("Stack based render: iterators, x_max is correct") {
    auto render = comp.get_stack_render();
    auto it = render.begin();
    auto row = *it;

    REQUIRE(it.x_max_ == 15);
  }

  SECTION("Stack based render: iterators, row size is correct") {
    auto render = comp.get_stack_render();
    auto it = render.begin();
    auto row = *it;

    REQUIRE(it.cache.sorted_line.size() == 6);
  }

  SECTION("Stack based render: iterators, check if a loop produces a count of "
          "1 rows") {
    auto render = comp.get_stack_render();
    int count = 0;
    for (auto row : render) {
      ++count;
    }

    REQUIRE(count == 1);
  }

  SECTION("Stack based render: complete render") {
    auto render = comp.get_stack_render();
    const char *letters =
        "0abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string text;
    int count = 0;
    for (auto rows : render) {
      for (auto cols : rows) {
        // std::println("stack empty():{} lastX:{}", rows.stack.empty(),
        // cols.xEnd);
        auto handle = cols.handle;

        for (auto index : cols.range) {
          text += letters[cols.handle.index()];
        }

        // std::println("count={} iterator={}", ++count, text);
      }
    }
    REQUIRE(text == expected);
  }
};

TEST_CASE("Multirow Scanline render", "[Multirow]") {

  using namespace term;
  using Rect = term::compositor::Rect;
  using X = term::compositor::X;
  using Y = term::compositor::Y;
  using Width = term::compositor::Width;
  using Height = term::compositor::Height;
  Compositor comp{15, 10};

  using rng = term::compositor::XPos;
  const std::string expected = "0aaa00000000000\n"
                               "0abbb0000000000\n"
                               "00bbbcccc000000\n"
                               "00bbbcccc000000\n"
                               "00bbbcccc000000\n"
                               "00bbbcdddddd000\n"
                               "00bbbceeeeeeee0\n"
                               "00bbbbeeeeeeee0\n"
                               "00bbbbeeeeeeee0\n"
                               "000000000000000\n";

  comp.new_layer(Rect{X{1}, Y{0}, Width{3}, Height{2}}, 1);
  comp.new_layer(Rect{X{2}, Y{1}, Width{3}, Height{8}}, 2);
  comp.new_layer(Rect{X{5}, Y{2}, Width{4}, Height{5}}, 1);
  comp.new_layer(Rect{X{6}, Y{5}, Width{6}, Height{1}}, 3);
  comp.new_layer(Rect{X{6}, Y{6}, Width{8}, Height{3}}, 2);

  SECTION("Stack based render: iterators, x_max is correct") {
    // auto render = comp.get_stack_render();
    // auto it = render.begin();
    // auto row = *it;
    //
    // REQUIRE(it.x_max_ == 15);
  }

  SECTION("Stack based render: iterators, row size is correct") {
    auto render = comp.get_stack_render();
    auto it = render.begin();
    auto row = *it;

    // REQUIRE(it.cache.sorted_line.size() == 3);
  }

  SECTION("Evalute inital y_") {
    auto render = comp.get_stack_render();
    REQUIRE(render.begin().y_ == 0);
  }

  SECTION("Evalute inital y_max") {
    auto render = comp.get_stack_render();
    REQUIRE(render.get_extents().y2 == 10);
  }
  SECTION("Stack based render: iterators, check if a loop produces a count of "
          "1 rows") {
    auto render = comp.get_stack_render();
    int count = 0;
    for (auto row : render) {
      ++count;
    }

    REQUIRE(count == 10);
  }

  SECTION("Stack based render: complete render") {
    auto render = comp.get_stack_render();
    const char *letters =
        "0abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string text;
    int count = 0;
    for (auto rows : render) {
      for (auto cols : rows) {
        // std::println("stack empty():{} lastX:{}", rows.stack.empty(),
        // cols.xEnd);
        for (auto index = cols.range.p; index < cols.range.p2; ++index) {

          text += letters[cols.handle.index()];
        }
        // std::println("count={} iterator={}", ++count, text);
      }
      text += '\n';
    }
    // std::println("{}", text);
    REQUIRE(text == expected); // text == expected);
  }
}
