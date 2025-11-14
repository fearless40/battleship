// #include "version.hpp"
#include "terminal/image.hpp"
#include "terminal/render.hpp"
#include <iostream>

void begin_game();

int main(int argv, char *argc[]) {

  using ColorOnly = term::details::Pixel_<term::Color>;
  ColorOnly pixels[10];

  for (unsigned char i = 0; i < 10; ++i) {
    if (i > 5) {
      pixels[i].red = 255 - i * 10;
      pixels[i].green = 100 + i * 10;
      pixels[i].blue = 20 * i;
    } else {
      pixels[i].red = 100;
      pixels[i].green = 100;
      pixels[i].blue = 100;
    }
    // pixels[i].value = 67 + i;
  };

  term::details::render(pixels);

  // std::cout << "Hello from the battleship program!\n";
  // std::cout << "Version: " << Version::MAJOR_VERSION << "."
  //           << Version::MINOR_VERSION << '\n';
  //
  return 0;
  begin_game();
  return 0;
}
