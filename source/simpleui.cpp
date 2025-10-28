#include <array>
#include <exception>
#include <format>
#include <iostream>

const std::array<const std::string, 3> AIChoices{"Random Guesser (Easy)",
                                                 "Basic Hunter (Medium)",
                                                 "Statistal Hunter (Hardest)"};

template <class DisplayFunction>
int get_user_integer(int min, int max, int def_value, DisplayFunction fn,
                     int try_count = 0) {
  fn();
  int value;

  if (try_count > 3)
    return def_value;

  try {
    std::cin >> value;
  } catch (std::exception &e) {
    std::cout << e.what() << '\n';
    return get_user_integer(min, max, def_value, fn, ++try_count);
  }

  if (std::cin.fail()) {
    std::cout << "Not valid input! \n";
    std::cin.clear();
    return get_user_integer(min, max, def_value, fn, ++try_count);
  }

  if (value < min || value > max) {
    std::cout << "Outside of range of allowed values: " << min << " to " << max
              << '\n';
    std::cout << "Try again \n";
    return get_user_integer(min, max, def_value, fn, ++try_count);
  }

  return value;
}

void begin_game() {
  // Only Plays Standard rules

  auto &out = std::cout;

  out << "Welcome to battleship.\n Please select an AI to play against. "
         "\n";

  int selection = get_user_integer(1, 3, 1, [&]() {
    std::size_t count{1};
    for (auto &ai : AIChoices) {
      out << count++ << ": " << ai << '\n';
    }

    out << "Selection: ";
  });

  out << "You chose: " << AIChoices[selection - 1];
}
