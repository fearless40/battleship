#include "actionresponse.hpp"
#include "ai/ai.hpp"
#include "ai/averageai.hpp"
#include "ai/bestai.hpp"
#include "ai/stupidai.hpp"
#include "gameserver/gamelayout.hpp"
#include "gameserver/ship.hpp"
#include "server.hpp"
#include "util/baseconv.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <format>
#include <ios>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
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

class PixelScreen {
public:
  enum class PixelFlags : unsigned char { reset_color = 0b00000001 };

private:
  struct Pixel {
    unsigned char flags;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    std::uint32_t unicode;
  };
};

// Callback is a function that returns bool : true if it drew somwthing and
// false if it did not. Callback accepts a RowCol
template <class CallBack>
void print_board(std::ostream &s, GameLayout const &layout,
                 CallBack &&callback) {
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
      if (!callback(RowCol(Row(row), Col(col))))
        s << colors::dark_gray << ".";

      s << colors::reset << " ";
    }
    s << el;
  }
}

void print_game_board(std::ostream &s, GameLayout const &layout,
                      Ship::Ships const &ships) {
  print_board(s, layout, [&](RowCol rc) {
    if (auto ship = Ship::ship_at_position(ships, rc); ship) {
      s << colors::red << ship.value().id().size;
      return true;
    }
    return false;
  });
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
    std::string_view display;
    OptionType otype;
    ShipDefinition shipid;
  };
  using namespace std::literals;

  using MenuAction = std::pair<std::string_view, ShipDefinition>;

  static const std::array<std::pair<std::string_view, ShipDefinition>, 2>
      menu_actions{MenuAction{"Randomize All"sv, ShipDefinition{1000}},
                   MenuAction{"Done"sv, ShipDefinition{1001}}};

  std::vector<MenuOptions> options;

  std::ranges::transform(
      std::views::iota(layout.minShipSize.size, layout.maxShipSize.size + 1),
      std::back_inserter(options), [](auto value) -> MenuOptions {
        return {.text = std::format("{} ({}Size={}{}) [{}Already Placed{}]",
                                    shipdef_to_name(ShipDefinition(value)),
                                    colors::light_gray, value, colors::reset,
                                    colors::green, colors::reset),
                .otype = MenuOptions::OptionType::ship,
                .shipid = ShipDefinition(value)};
      });
  std::ranges::transform(menu_actions, std::back_inserter(options),
                         [](auto const value) -> MenuOptions {
                           return {.text = std::string(value.first),
                                   .otype = MenuOptions::OptionType::action,
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

  while (true) {
    print_game_board(std::cout, layout, ships);

    // Update the menu to have the correct text to display
    for (auto &menu : options) {
      menu.display = menu.text;
      if (auto found = std::ranges::find(ships, menu.shipid, &Ship::id);
          found == ships.end()) {
        // Ship not found so display partial text
        if (auto pos = menu.text.find(" ["); pos != std::string::npos)
          menu.display = {menu.text.begin(), menu.text.begin() + pos};
      }
    }

    auto selection =
        menu_choice(options, "Select Option: ", [](auto const &value) {
          return value.display;
        });

    switch (selection->otype) {
    case MenuOptions::OptionType::action:
      if (selection->shipid == ShipDefinition(1000)) {
        if (auto ships_opt = Ship::random_ships(layout); ships_opt)
          ships = ships_opt.value();
        continue;
      } else if (selection->shipid == ShipDefinition(1001)) {
        return ships;
      }
    case MenuOptions::OptionType::ship: {
      // Ask for ship position
      auto ship_id = selection->shipid.size;

      std::print(colors::reset);
      std::println("Place the ship by typing in the placement by using ColRow "
                   "style excel input. For instance a8.");
      std::print("Please input: {}", colors::light_gray);
      std::string row_col, orientation;
      std::cin >> row_col;
      std::println("Please choose orientation: {}v{} for Vertical, {}h{} for "
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
    }
    }

    if (Ship::any_collisions(ships) ||
        std::ranges::none_of(
            ships, [&](auto &ship) { return ship.is_placed_valid(layout); })) {
      std::println(
          "{}Ships off the map or colliding will reset the last chosen "
          "ship. Press any key to continue.{}",
          colors::light_gray, colors::reset);
      std::cin.get();
      if (auto shipFound =
              std::ranges::find(ships, selection->shipid, &Ship::id);
          shipFound != ships.end()) {
        auto it = ships.erase(shipFound);
      }
    }
    std::print("{}{}", erase::all, move::home);
  }
  return {};
}

void human_play(ClassicGameServer::ClientPlayer &p, const GameLayout &layout) {
  // Clear the screen
  std::print(erase::all);
  std::print(move::home);

  std::print(colors::green);
  std::println("Your turn -- Round: {}{}{}", colors::red, p.round(),
               colors::reset);

  std::println("Your guesses: ");
  std::println("---------------------------------------------------------");
  auto history = p.prior_shots();
  print_board(std::cout, layout, [&](RowCol rc) {
    auto it =
        std::ranges::find(history, rc, &ClassicGameServer::History::first);
    if (it == history.end())
      return false;

    if (it->second.is_hit())
      std::print("{}{}", colors::green, 'X');
    else if (it->second.is_sunk())
      std::print("{}{}", colors::green, it->second.ship.size);
    else if (it->second.is_miss())
      std::print("{}{}", colors::red, 'O');

    return true;
  });

  auto player_ship_health = p.ships_all_hits();
  for (auto &status : player_ship_health) {
    std::print("{}: Number of hits: {}{}{}/{}", shipdef_to_name(status.id),
               colors::red, status.number_hits, colors::reset, status.id.size);
    if (status.sunk)
      std::print("  {}SUNK{}", colors::red, colors::reset);
    std::print("\n");
  }

  while (1) {
    std::print("Enter your next guess: {}", colors::green);
    std::string user_input;
    std::cin >> user_input;

    if (user_input == "quit")
      std::terminate();

    if (auto rc_opt = RowCol::from_string_opt(user_input); rc_opt) {
      if (!layout.is_row_col_valid(rc_opt.value())) {
        std::println("Invalid value entered");
        continue;
      }

      if (!p.fire_single_shot(rc_opt.value())) {
        std::println("Shot invalid per game.");
        continue;

      } else
        return;

    } else {
      std::println("Unable to recognize the value entered. Please try again.");
      continue;
    }
  }
}

void play(const GameLayout &layout, Ship::Ships &&playerShips,
          Ship::Ships &&aiShips, std::unique_ptr<AI> ai) {
  const auto HUMAN_ID = UserID(1);
  const auto AI_ID = UserID(2);

  ClassicGameServer server{layout};

  if (auto human_player_opt =
          server.set_player(HUMAN_ID, std::move(playerShips));
      !human_player_opt)
    return;

  if (auto ai_player_opt = server.set_player(AI_ID, std::move(aiShips));
      !ai_player_opt)
    return;

  while (server.next_round()) {
    auto player = server.get_current_player();
    if (player.user_id() == HUMAN_ID) {
      human_play(player, layout);
    }
    if (player.user_id() == AI_ID) {
      auto guess = ai->guess();
      if (guess) {
        if (!player.fire_single_shot(guess.value_or(RowCol{Row{0}, Col{0}})))
          return;
        auto response = player.last_response();
        switch (response.value) {
          using enum Response::ResponseValue;
        case hit:
          ai->hit();
          break;
        case miss:
          ai->miss();
          break;
        case sink:
          ai->sink(response.ship);
          break;
        default:
          ai->miss();
          break;
        }
      }
    }
  }
  std::println("Game over!!");
  if (auto winner_opt = server.winner(); winner_opt) {
    if (winner_opt.value().user_id() == HUMAN_ID)
      std::println("You WON! Nice job.");
    else
      std::println("You LOST that sucks. Try again...");

  } else
    std::println("The game cannot figure out who won!");
}

void begin_game() {
  // Only Plays Standard rules

  std::println("Welcome to battleship.\n Please select an AI to play against.");

  auto choice =
      menu_choice(AIChoices, "Select AI", [](auto const &v) { return v; });
  std::println("You chose: {}", *choice);

  std::print(erase::all);

  GameLayout layout;
  Ship::Ships ships;

  std::print(move::home);
  auto ships_opt = place_ships(layout);
  if (ships_opt)
    ships = std::move(ships_opt.value());
  if (ships.size() == 4) {
    std::print("{}{}", erase::all, move::home);
    print_game_board(std::cout, layout, ships);
  }

  std::unique_ptr<AI> ai_ptr;

  auto pos = std::distance(AIChoices.begin(), choice);

  switch (pos) {
  default:
  case 0:
    ai_ptr = std::make_unique<StupidAI>();
    break;
  case 1:
    ai_ptr = std::make_unique<AverageAI>();
    break;
  case 2:
    ai_ptr = std::make_unique<BestAI>();
    break;
  }

  ai_ptr->new_game(layout);

  if (auto aiShip = Ship::random_ships(layout); aiShip)
    play(layout, std::move(ships), std::move(aiShip.value()),
         std::move(ai_ptr));
}
