#include "compositor.hpp"
#include <catch2/catch_test_macros.hpp>
#include <print>

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
    std::println("Count={}, Text={}", ++count, output);
  }

  return output;
}

TEST_CASE("ScanlineRenders", "[scanlinerender]") {

  using namespace term;
  Compositor comp{15, 0};

  const std::string expected = "0ab00cdec000000";

  comp.new_layer(util::IntRect{1, 0, 1, 0}, 1);
  comp.new_layer(util::IntRect{2, 0, 1, 0}, 2);
  comp.new_layer(util::IntRect{5, 0, 4, 0}, 1);
  comp.new_layer(util::IntRect{6, 0, 1, 0}, 3);
  comp.new_layer(util::IntRect{6, 0, 2, 0}, 2);

  SECTION("Stack based render") {
    auto render = comp.get_stack_render_old();
    render.init_line();
    REQUIRE(convert_output_to_string(render) == expected);
  }

  SECTION("Painter based render") {

    auto render = comp.get_painter_render();
    render.init_line();
    REQUIRE(convert_output_to_string(render) == expected);
  }

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
          "9 rows") {
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
        std::println("stack empty():{} lastX:{}", rows.stack.empty(),
                     cols.xEnd);
        for (int index = cols.begin(); index < cols.end(); ++index) {
          text += letters[cols.handle.index()];
        }

        std::println("count={} iterator={}", ++count, text);
      }
    }
    REQUIRE(text == expected);
  }
}
