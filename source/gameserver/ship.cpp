#include "ship.hpp"
#include "collisions.hpp"
#include "randomutil.hpp"
// #include <terminal.hpp>

namespace battleship {

std::optional<Ship> ship_at_position(Ships const &ships, RowCol pos) {
  for (auto &ship : ships) {
    int y = (int)pos.row.size;
    int x = (int)pos.col.size;
    if (ship.location.contains_point(x, y))
      return ship;
  }
  return {};
}

std::optional<Ships> random_ships(GameLayout const &game) {

  // Validate that it is a valid game board
  if (!game.is_valid())
    return {};

  Ships ships;
  ships.reserve(game.maxShipSize.size - game.minShipSize.size + 1);

  for (ShipDefinition ship_id = game.minShipSize;
       ship_id.size < game.maxShipSize.size + 1; ++ship_id.size) {
    // std::cout << "Attempting to build placement for ship: " << ship_id.size
    // << '\n';

    ships.emplace_back(ship_id, AABB{});
    // std::cout << "Ships array: " << ships.size() << '\n';

    for (size_t count_attempts = 0; count_attempts < 400; ++count_attempts) {
      Orientation layout = randomns::coin_flip() == true
                               ? Orientation::Horizontal
                               : Orientation::Vertical;
      std::uint16_t width =
          (layout == Orientation::Vertical ? 0 : ship_id.size - 1);
      std::uint16_t height =
          (layout == Orientation::Horizontal ? 0 : ship_id.size - 1);

      int row = randomns::between(0, game.nbrRows.size - 1 - height);
      int col = randomns::between(0, game.nbrCols.size - 1 - width);
      ships.back().location = AABB{col, row, col + width, row + height};
      if (!any_collisions(ships)) {
        // std::cout << "No collision placed ship\n";
        // std::format("Player {} generated: {} after {} tries", name,
        // ship_id, count_attempts));
        break;
      } else {
        // std::cout << "Ships collided.\n";
      }
    }
  }
  if (any_collisions(ships)) {
    // std::cout << "Collisions still after placement.\n";
    return {};
  }

  return ships;
}

} // namespace battleship
