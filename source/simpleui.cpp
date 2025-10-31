#include "gameserver/gamelayout.hpp"
#include "gameserver/ship.hpp"
#include "util/baseconv.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <exception>
#include <format>
#include <ios>
#include <iostream>
#include <iterator>
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

template <std::ranges::random_access_range Range, typename Function>
auto menu_choice(Range &&menuItems, const std::string &prompt, Function &&fn) {
  int attemps = 0;
  do {

    for (auto const [index, value] : std::views::enumerate(menuItems)) {
      std::println("{}{}{} {}", colors::green, index + 1, colors::reset,
                   fn(value));
    }

    std::print("{}: {}", prompt, colors::green);
    int choice{0};
    std::cin >> choice;

    if (std::cin.fail()) {
      std::println("Failed to parse the input. Please try again.");
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      choice = 0;
    }

    if (choice >= 1 and choice <= menuItems.size()) {
      return menuItems.begin() + (choice - 1);
    }

    std::println(
        "Invalid choice: {}{}{} must choose number between {}1{} to {}{}{}",
        colors::red, choice, colors::reset, colors::green, colors::reset,
        colors::green, menuItems.size(), colors::reset);
    std::println("Attempt {} out of 3", ++attemps);
  } while (attemps < 3);

  return menuItems.end();
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
  struct MenuOptions {
    enum class OptionType { ship, action };
    std::string text;
    OptionType otype;
    ShipDefinition shipid;
    bool already_chosen{false};
  };
  using namespace std::literals;

  using MenuAction = std::pair<std::string_view, ShipDefinition>;

  static const std::array<std::pair<std::string_view, ShipDefinition>, 2>
      menu_actions{MenuAction{"Randomize All"sv, ShipDefinition{0}},
                   MenuAction{"Quit"sv, ShipDefinition{1}}};

  std::vector<MenuOptions> options;

  std::ranges::transform(
      std::views::iota(layout.minShipSize.size, layout.maxShipSize.size + 1),
      std::back_inserter(options), [](auto value) -> MenuOptions {
        return {.text = shipdef_to_name(ShipDefinition(value)),
                .otype = MenuOptions::OptionType::ship,
                .shipid = ShipDefinition(value)};
      });
  std::ranges::transform(menu_actions, std::back_inserter(options),
                         [](auto const value) -> MenuOptions {
                           return {.text = std::string(value.first),
                                   .otype = MenuOptions::OptionType::ship,
                                   .shipid = value.second};
                         });

  std::print(colors::reset);
  std::println(
      "Lets place your ships. Select the ship you would like to place.");

  Ship::Ships ships;

  const std::size_t nbrShips =
      layout.maxShipSize.size - layout.minShipSize.size + 1;
  const auto min_size = layout.minShipSize.size;
  const auto max_size = layout.maxShipSize.size;

  ships.reserve(nbrShips);

  while (ships.size() < nbrShips) {
    print_game_board(std::cout, layout, ships);

    // Update the menu to have the correct text to display
    for (auto const &shipid : ships) {
      auto it = std::ranges::find_if(options, [](auto cosnt &opt) {
        opt.shipid == shipid &&opt.otype ==
            MenuOptions::OptionType::ship &&opt.already_chosen == false;
      });
      if (it != options.end()) {
            *it.text = std::format("{} [{}Placed{}] [{}Ship Size: {}{}], shipdef_to_name(*it.shipid), colors::light_gray, colors::reset, colors::light_gray, opt.shipid.size, colors::reset); 
            *it
            
   

    int menuSelection = get_user_integer(
        min_size, max_size + extra_options.size(), min_size, [&]() {
          std::println("Ships to select: ");
          for (int shipID : std::ranges::views::iota(
                   min_size, max_size + 1 + extra_options.size())) {
            const std::string optName =
                shipID > max_size ? extra_options[shipID - nbrShips - min_size]
                                  : shipdef_to_name(ShipDefinition(shipID));
            std::print("{}{}{} {}", colors::green, shipID - min_size + 1,
                       colors::reset, optName);

            if (shipID <= max_size)
              std::print(" [Ship Size: {}{}{}]", colors::light_gray, shipID,
                         colors::reset);

            if (std::ranges::find(ships, ShipDefinition(shipID), &Ship::id) !=
                ships.end())
              std::println("{} [Already placed]{}", colors::green,
                           colors::reset);
            else
              std::println("");
          }

          std::print("{}Select number: {}", colors::reset, colors::light_gray);
        });

    if (auto ship_id = menuSelection + min_size; ship_id > max_size) {
          // In extra menu options
          auto extra_option = menuSelection - max_size;
          switch (extra_option) {
          case 0: // Randomize
          {
            auto ships_opt = Ship::random_ships(layout);
            if (ships_opt)
              ships = std::move(ships_opt.value());

          } break;
          case 1: // Quit
            return {};
          }
    } else {
          // Ask for ship position

          std::print(colors::reset);
          std::println(
              "Place the ship by typing in the placement by using ColRow "
              "style excel input. For instance a8.");
          std::print("Please input: {}", colors::light_gray);
          std::string row_col, orientation;
          std::cin >> row_col;
          std::println(
              "Please choose orientation: {}v{} for Vertical, {}h{} for "
              "Horizontal",
              colors::green, colors::reset, colors::green, colors::reset);
          std::print("(v) or h {}", colors::light_gray);
          std::cin >> orientation;

          auto rc = RowCol::from_string(row_col);
          AABB pos;

          if (orientation == "" || orientation == "v")
            pos = {rc.col.size, rc.row.size, rc.col.size,
                   static_cast<int>(rc.row.size + ship_id - 1)};
          else
            pos = {rc.col.size, rc.row.size,
                   static_cast<int>(rc.col.size + ship_id - 1), rc.row.size};

          if (auto shipFound =
                  std::ranges::find(ships, ShipDefinition(ship_id), &Ship::id);
              shipFound != ships.end())
            shipFound->location = pos;
          else
            ships.emplace_back(ShipDefinition(ship_id), pos);

          if (Ship::any_collisions(ships) ||
              std::ranges::none_of(ships, [&](auto &ship) {
                return ship.is_placed_valid(layout);
              })) {
            std::println(
                "{}Ships off the map or colliding will reset the last chosen "
                "ship. Press any key to continue.{}",
                colors::light_gray, colors::reset);
            std::cin.get();
            if (auto shipFound = std::ranges::find(
                    ships, ShipDefinition(ship_id), &Ship::id);
                shipFound != ships.end()) {
              auto it = ships.erase(shipFound);
            }
          }
    }

    std::print("{}{}", erase::all, move::home);
      }
      return {};
    }

    void begin_game() {
      // Only Plays Standard rules

      std::println(
          "Welcome to battleship.\n Please select an AI to play against.");

      auto choice =
          menu_choice(AIChoices, "Select AI", [](auto const &v) { return v; });
      std::println("You chose: {}", *choice);

      return;

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
      // ships = place_ships(layout);
      if (ships.size() > 0) {
        std::print("{}{}", erase::all, move::home);
        print_game_board(std::cout, layout, ships);
      }
    }
