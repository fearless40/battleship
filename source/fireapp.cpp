#include "compositor_shared.hpp"
#include "keyparser.hpp"
#include "pixel.hpp"
#include <chrono>
#include <compositor.hpp>
#include <ctime>
#include <image.hpp>
#include <iostream>
#include <print>
#include <ranges>
#include <render.hpp>
#include <term_control.hpp>

namespace pixel {
using namespace term::pixel;
using Pix = Pixel<pixel::ASCII, pixel::Color>;
} // namespace pixel

term::Image<pixel::Pix> color{term::compositor::Width{30},
                              term::compositor::Height{30}};

int offset{0};
unsigned char buffer[8096];

void render_frame() {
  unsigned char blue = 90;
  unsigned char step = 5;

  auto output = std::span<unsigned char>(buffer, 8096);

  for (auto row : color.rows()) {
    for (auto [col, pixel] : std::views::enumerate(row)) {
      pixel.blue = (blue + step * col) + offset;
      pixel.green = pixel.blue;
      pixel.red = pixel.blue;
    }
  }

  offset++;
  offset = std::clamp(offset, 0, 200);
  term::details::render_to_buffer(color, output);

  std::cout << buffer;
  std::cout << "\e[H";
};

int main(int argv, char *arguments[]) {
  using namespace std::chrono_literals;
  constexpr const std::chrono::milliseconds milli_per_frame = 1000ms / 20;
  auto start_time = std::chrono::steady_clock::now();
  bool loop = true;

  for (auto row : color.rows()) {
    std::ranges::fill(row, pixel::Pix{'X', {0, 0, 0}});
  }

  std::println("Starting...");

  term::TermControl term;
  while (loop) {
    term.on_loop();
    auto next_time = std::chrono::steady_clock::now();
    if (std::chrono::milliseconds frame_time =
            std::chrono::duration_cast<std::chrono::milliseconds>(next_time -
                                                                  start_time);
        frame_time > milli_per_frame) {
      render_frame();
      start_time = std::chrono::steady_clock::now();
    }

    if (term.had_key_event()) {
      if (auto key = term.get_key_event(); key.key == 'a') {

        std::println("Key pressed: {}", (char)key.key);
        loop = false;
      }
    }
  }

  std::println("Ending...");
}
