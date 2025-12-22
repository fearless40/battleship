// #include "version.hpp"
#include "soa.hpp"
#include "soamemorylayout.hpp"
#include "terminal/image.hpp"
#include "terminal/render.hpp"
#include "terminal/term_control.hpp"
#include <iostream>
#include <print>
#include <ranges>
#include <stack>
void begin_game();

int main(int argv, char *argc[]) {

  // util::soa::SOA<util::soa::DynamicArray, char, int> soa;
  // {
  //   util::soa::SOA<util::soa::memory_layout::FixedArray<100>, char, int> soa;
  //
  //   soa.push_back('a', 1);
  //   soa.push_back('b', 2);
  //   soa.push_back('c', 3);
  //   soa.push_back('d', 4);
  //
  //   std::println("Capacity: {}, Size: {}", soa.capacity(), soa.size());
  // }
  // {
  //   util::soa::SOA<util::soa::memory_layout::DynamicArray, char, int> soa;
  //
  //   soa.push_back('a', 1);
  //   soa.push_back('b', 2);
  //   soa.push_back('c', 3);
  //   soa.push_back('d', 4);
  //
  //   for (int i = 1; i < 250; ++i) {
  //     soa.push_back('a', i);
  //   }
  //
  //   soa.remove(soa.end() - 1);
  //
  //   std::println("Capacity: {}, Size: {}", soa.capacity(), soa.size());
  // }
  //
  struct layer {
    char letter{'0'};
    int x{0}, x2{0};
    int z{0};
  };

  std::stack<layer, std::vector<layer>> stack;
  std::vector<layer> layers;

  layers.push_back({'0', 0, 11, 0});
  layers.push_back({'a', 1, 2, 1});
  layers.push_back({'b', 2, 3, 2});
  layers.push_back({'c', 5, 9, 1});
  layers.push_back({'e', 6, 6, 3});
  layers.push_back({'d', 6, 7, 2});

  std::string expected = "0abb0cedcc00";
  std::string position = "0123456789ab";

  std::string output{};

  std::vector<layer>::iterator next = layers.begin() + 1;
  auto end = layers.end();
  stack.push(*layers.begin());

  int x = 0, x2 = 0;
  char l = '@';
  int count = 0;

  auto print_stack = [&]() {
    auto stack_new = stack;
    std::println("Stack sz: {} \n-----", stack_new.size());
    while (stack_new.size() > 0) {
      auto it = stack_new.top();
      std::println("[{},{},{},{}]", it.letter, it.x, it.x2, it.z);
      stack_new.pop();
    }

    std::println("-------");
  };

  auto pop_stack = [&]() {
    while (stack.size() > 0 and stack.top().x2 < x2) {
      // std::println("Pop: L={}, X={} X2={} Z={}", stack.top().letter,
      // stack.top().x, stack.top().x2, stack.top().z);
      stack.pop();
    }
  };

  while (stack.size() > 0 && count < 35) {
    std::println("L={} X={} X2={}", l, x, x2);
    print_stack();

    l = stack.top().letter;
    if (x < stack.top().x) {
      x = stack.top().x;

    } else {
      x = x2;
    }

    if (next != end)
      std::println("NEXT L={} X={} X2={}", next->letter, next->x, next->x2);
    else
      std::println("END");

    // Skip items that are covered up by items on top
    while (next != end and x2 > next->x2) {
      ++next;
    }

    if (next == end || (next->z < stack.top().z and x2 < stack.top().x2)) {
      x2 = stack.top().x2 + 1;
    } else {
      x2 = next->x;
      if (next != end) {
        stack.push(*next);
        ++next;
      }
    }

    pop_stack();
    std::println("CURR L={} X={} X2={}", l, x, x2);

    if (x2 - x == 0) {
      output += l;
      ++x2;
    } else
      for (int i = x; i < x2; ++i) {
        output += l;
      }
    std::println("Output:   {}", output);
    std::println("Expected: {}", expected);
    std::println("Position: {}", position);
    l = '@';
    ++count;
  }

  // std::cout << output;
  //
  // term::TermControl tc{};
  // using ColorOnly = term::details::Pixel_<term::Color, term::ASCII>;
  // term::Image<ColorOnly> picture(10, 10);
  //
  // for (unsigned int x = 0; x < picture.width(); ++x) {
  //   for (unsigned int y = 0; y < picture.height(); ++y) {
  //     ColorOnly p;
  //     p.red = y * 10;
  //     p.green = x * 10;
  //     p.blue = x * y;
  //     p.value = 'a' + x;
  //
  //     picture.set_pixel(x, y, p);
  //   }
  // }
  //
  // unsigned char buffer[4096];
  //
  // term::details::render_to_buffer(picture, buffer);
  //
  // std::cout << buffer; //<< std::endl;
  //
  // while (1) {
  //   tc.on_loop();
  //
  //   // sleep(1);
  // }

  // std::cout << "Hello from the battleship program!\n";
  // std::cout << "Version: " << Version::MAJOR_VERSION << "."
  //           << Version::MINOR_VERSION << '\n';
  //
  return 0;
  begin_game();
  return 0;
}
