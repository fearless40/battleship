#include "version.hpp"
#include <iostream>

int main(int argv, char *argc[]) {

  std::cout << "Hello from the battleship program!\n";
  std::cout << "Version: " << Version::MAJOR_VERSION << "."
            << Version::MINOR_VERSION << '\n';
  return 0;
}
