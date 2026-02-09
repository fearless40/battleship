#include "keyparser.hpp"
#include <chrono>
#include <compositor.hpp>
#include <ctime>
#include <print>
#include <term_control.hpp>

void render_frame() {};

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
