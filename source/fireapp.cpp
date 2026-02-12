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

term::Image<term::pixel::Pixel<term::pixel::BgColor>> bgColor{
    term::compositor::Width{30}, term::compositor::Height{30}};

int offset{0};
unsigned char buffer[8096];

void render_frame() {
  unsigned char blue = 90;
  unsigned char step = 5;

  auto output = std::span<unsigned char>(buffer, 8096);

  for (auto row : bgColor.rows()) {
    for (auto [col, pixel] : std::views::enumerate(row)) {
      pixel.bgblue = (blue + step * col) + offset;
      pixel.bggreen = 0;
      pixel.bgred = 0;
    }
  }

  term::details::render_to_buffer(bgColor, output);

  std::cout << buffer;
  std::cout << "\e[H";
};

int main(int argv, char *arguments[]) {

  constexpr const float milli_per_frame = 1000.f / 60.f;
  auto start_time = std::chrono::steady_clock::now();
  bool loop = true;

  std::println("Starting...");

  term::TermControl term;
  while (loop) {
    term.on_loop();
    auto next_time = std::chrono::steady_clock::now();
    if (std::chrono::milliseconds frame_time =
            std::chrono::duration_cast<std::chrono::milliseconds>(next_time -
                                                                  start_time);
        frame_time.count() > milli_per_frame) {
      render_frame();
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
