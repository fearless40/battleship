// #include "version.hpp"
#include "terminal/image.hpp"
#include "terminal/render.hpp"
#include "terminal/term_control.hpp"
#include <iostream>

void begin_game();

int main(int argv, char *argc[]) {

  term::TermControl tc{};
  using ColorOnly = term::details::Pixel_<term::Color, term::ASCII>;
  term::Image<ColorOnly> picture(10, 10);

  for (unsigned int x = 0; x < picture.width(); ++x) {
    for (unsigned int y = 0; y < picture.height(); ++y) {
      ColorOnly p;
      p.red = y * 10;
      p.green = x * 10;
      p.blue = x * y;
      p.value = 'a' + x;

      picture.set_pixel(x, y, p);
    }
  }

  unsigned char buffer[4096];

  term::details::render_to_buffer(picture, buffer);

  std::cout << buffer; //<< std::endl;

  while (1) {
    tc.on_loop();

    // sleep(1);
  }

  // std::cout << "Hello from the battleship program!\n";
  // std::cout << "Version: " << Version::MAJOR_VERSION << "."
  //           << Version::MINOR_VERSION << '\n';
  //
  return 0;
  begin_game();
  return 0;
}
