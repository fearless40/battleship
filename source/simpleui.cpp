#include "gameserver/gamelayout.hpp"
#include "gameserver/ship.hpp"
#include "util/baseconv.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <exception>
#include <format>
#include <iostream>
#include <limits>
#include <numeric>
#include <optional>
#include <print>
#include <ranges>
#include <string>
#include <string_view>

namespace term {
namespace details {

constexpr char *to_chars(char *start, char *end, int value) {

  if (value > 9999 || value < 0)
    return start;

  int nbrChars = std::numeric_limits<int>::digits10;

  if (value <= 9)
    nbrChars = 1;
  else if (value <= 99)
    nbrChars = 2;
  else if (value <= 999)
    nbrChars = 3;
  else if (value <= 9999)
    nbrChars = 4;

  if (nbrChars + start > end)
    return start;

  int i = nbrChars;
  do {
    start[--i] = '0' + (value % 10);
    value /= 10;
  } while (value != 0);
  return &start[nbrChars];
}
} // namespace details
} // namespace term

namespace colors {

#define COLORF(r, g, b) ("\e[38;2;" #r ";" #g ";" #b "m")

constexpr const std::string_view red = COLORF(200, 0, 0);
constexpr const std::string_view green = COLORF(0, 200, 0);
constexpr const std::string_view blue = COLORF(0, 0, 200);
constexpr const std::string_view reset = "\e[0m";
constexpr const std::string_view light_gray = COLORF(190, 190, 190);
constexpr const std::string_view dark_gray = COLORF(90, 90, 90);

#undef COLORF

} // namespace colors
//

namespace move {

#define MOVE(letter) ("\e[" #letter)
constexpr const std::string home = MOVE(H);

constexpr const std::string position(int line, int col) {
  char command[2 + 4 + 1 + 4 + 1] = "\e[";
  char *next = &command[2];
  char *end = &command[11];
  next = term::details::to_chars(next, end, line);
  *next = ';';
  next = term::details::to_chars(next, end, col);
  *next = 'H';
  *(++next) = 0;
  return command;
}

constexpr const std::string relative(int value, char motion) {
  char command[2 + 4 + 1 + 1] = "\e[";
  char *next = &command[2];
  char *end = &command[7];
  next = term::details::to_chars(next, end, value);
  *next = motion;
  *(++next) = 0;
  return command;
}

constexpr const std::string up(int amount) { return relative(amount, 'A'); }

constexpr const std::string down(int amount) { return relative(amount, 'B'); }

constexpr const std::string right(int amount) { return relative(amount, 'C'); }

constexpr const std::string left(int amount) { return relative(amount, 'D'); }

constexpr const std::string down_front(int amount) {
  return relative(amount, 'E');
}

constexpr const std::string up_front(int amount) {
  return relative(amount, 'F');
}

constexpr const std::string column(int amount) { return relative(amount, 'G'); }
} // namespace move

namespace erase {

constexpr std::string cur_endscreen = "\e[0J";
constexpr std::string cur_begscreen = "\e[1J";
constexpr std::string all = "\e[2J";
constexpr std::string cur_endline = "\e[0K";
constexpr std::string cur_begline = "\e[1K";
constexpr std::string line = "\e[2K";
} // namespace erase

const std::array<const std::string, 3> AIChoices{"Random Guesser (Easy)",
                                                 "Basic Hunter (Medium)",
                                                 "Statistal Hunter (Hardest)"};

const std::array<const std::string, 4> ShipNames{"Destroyer", "Submarine",
                                                 "Battleship", "Carrier"};

template <class DisplayFunction>
int get_user_integer(int min, int max, int def_value, DisplayFunction &&fn) {

  for (int try_count = 0; try_count != 3; ++try_count) {

    fn();
    int value{def_value};

    if (try_count > 3)
      return def_value;

    try {
      std::cin >> value;
    } catch (std::exception &e) {
      std::println("Error: {}", e.what());
      continue;
    }

    if (std::cin.fail()) {
      std::println("{}Not valid input!{}", colors::red, colors::reset);
      std::cin.clear();
      std::cin.ignore();
      continue;
    }

    if (value < min || value > max) {
      std::println("{}Outside of range of allowed values: {}{} {}to {}{}{}",
                   colors::reset, colors::green, min, colors::reset,
                   colors::green, max, colors::reset);
      std::println("Try again");
      continue;
    }

    return value;
  }

  return def_value;
}

struct repeat {
  constexpr explicit repeat(std::size_t count, const std::string_view &value)
      : m_count(count), m_value(value) {}

  friend std::ostream &operator<<(std::ostream &s, const repeat &rp) {
    for (std::size_t c = 0; c < rp.m_count; ++c) {
      s << rp.m_value;
    }
    return s;
  }

private:
  const std::size_t m_count;
  const std::string_view m_value;
};

void print_game_board(std::ostream &s, GameLayout const &layout,
                      Ship::Ships const &ships) {
  // Write header
  //    A B C D E F G H I J K L
  //    _______________
  //  1|. 2 . . . . . . . . . .
  //   |
  //  2|
  //   |
  //  3|
  //   |
  // 10|

  const char el = '\n';

  s << "    " << colors::reset; // two blank spaces for header
  for (std::size_t col = 0; col != layout.nbrCols.size; ++col) {
    s << base26::to_string(static_cast<int>(col)) << ' ';
  }
  s << el << colors::reset;

  // Col line
  s << "   " << "┌" << repeat(layout.nbrCols.size * 2, "─") << el;

  // Rows
  for (std::size_t row = 0; row < layout.nbrRows.size; ++row) {
    if (row < 10)
      s << "  ";
    else
      s << " ";
    s << row << "│";
    for (std::size_t col = 0; col < layout.nbrCols.size; ++col) {
      if (auto ship = Ship::ship_at_position(
              ships, RowCol{Row{static_cast<unsigned short>(row)},
                            Col{static_cast<unsigned short>(col)}});
          ship) {
        s << colors::red << ship.value().id().size;
      } else {
        s << colors::dark_gray << ".";
      }
      s << colors::reset << " ";
    }
    s << el;
  }
}

const std::string shipdef_to_name(ShipDefinition def) {
  auto sz = def.size - 2;
  if (sz < 0 || sz > 4)
    return "Unknown ship!";
  return ShipNames[sz];
}

std::optional<Ship::Ships> place_ships(GameLayout const &layout) {
  std::print(colors::reset);
  std::println(
      "Lets place your ships. Select the ship you would like to place.");

  Ship::Ships ships;

  const std::size_t nbrShips =
      layout.maxShipSize.size - layout.minShipSize.size + 1;
  ships.reserve(nbrShips);

  while (ships.size() < nbrShips) {

    print_game_board(std::cout, layout, ships);
    int ship_id = get_user_integer(
        layout.minShipSize.size, layout.maxShipSize.size,
        layout.minShipSize.size, [&]() {
          std::println("Ships to select: ");
          for (int shipID : std::ranges::views::iota(
                   layout.minShipSize.size, layout.maxShipSize.size + 1)) {
            std::print("{}{}{} {}", colors::green, shipID, colors::reset,
                       shipdef_to_name(ShipDefinition(shipID)));
            if (std::ranges::find(ships, ShipDefinition(shipID), &Ship::id) !=
                ships.end())
              std::println("{} [Already placed]{}", colors::green,
                           colors::reset);
            else
              std::println("");
          }

          std::print("{}Select number: {}", colors::reset, colors::light_gray);
        });

    std::print(colors::reset);
    std::println("Place the ship by typing in the placement by using ColRow "
                 "style excel input. For instance a8.");
    std::print("Please input: {}", colors::light_gray);
    std::string row_col, orientation;
    std::cin >> row_col;
    std::println(
        "Please choose orientation: {}v{} for Vertical, {}h{} for Horizontal",
        colors::green, colors::reset, colors::green, colors::reset);
    std::print("(v) or h {}", colors::light_gray);
    std::cin >> orientation;

    auto rc = RowCol::from_string(row_col);
    AABB pos;

    if (orientation == "" || orientation == "v")
      pos = {rc.col.size, rc.row.size, rc.col.size, rc.row.size + ship_id - 1};
    else
      pos = {rc.col.size, rc.row.size, rc.col.size + ship_id - 1, rc.row.size};

    if (auto shipFound =
            std::ranges::find(ships, ShipDefinition(ship_id), &Ship::id);
        shipFound != ships.end())
      shipFound->location = pos;
    else
      ships.emplace_back(ShipDefinition(ship_id), pos);

    if (Ship::any_collisions(ships) ||
        std::ranges::none_of(
            ships, [&](auto &ship) { return ship.is_placed_valid(layout); })) {
      std::println(
          "{}Ships off the map or colliding will reset the last chosen "
          "ship. Press any key to continue.{}",
          colors::light_gray, colors::reset);
      std::cin.get();
      if (auto shipFound =
              std::ranges::find(ships, ShipDefinition(ship_id), &Ship::id);
          shipFound != ships.end()) {
        auto it = ships.erase(shipFound);
      }
    }

    std::print("{}{}", erase::all, move::home);
  }
  return {};
}

void begin_game() {
  // Only Plays Standard rules

  std::println("Welcome to battleship.\n Please select an AI to play against.");

  int selection = get_user_integer(1, 3, 1, [&]() {
    std::size_t count{1};
    for (auto &ai : AIChoices) {
      std::println("{}{}{}: {}", colors::green, count++, colors::reset, ai);
    }

    std::print("Selection: {}", colors::green);
  });

  std::println("{}You chose: {}{}{}", colors::reset, colors::green,
               AIChoices[selection - 1], colors::reset);

  std::print(erase::all);

  GameLayout layout;
  Ship::Ships ships;

  std::print(move::home);
  place_ships(layout);
}
