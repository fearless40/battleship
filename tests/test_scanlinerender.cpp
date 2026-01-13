#include "compositor.hpp"
#include <catch2/catch_test_macros.hpp>

template <typename RENDER>
std::string convert_output_to_string(RENDER &render) {
  std::string output;
  const char *letters = "0abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

  for (int i = 0; i < 10; ++i) {
    auto [handle, x, x2] = render.line_next();
    for (int pos = x; pos < x2; ++pos) {
      output += letters[handle.index()];
    }
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
    auto render = comp.get_scanline_render();
    render.init_line();
    REQUIRE(convert_output_to_string(render) == expected);
  }

  SECTION("Painter based render") {

    auto render = comp.get_scanline_painter();
    render.init_line();
    REQUIRE(convert_output_to_string(render) == expected);
  }
}
