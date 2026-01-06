#pragma once
#include "collisions.hpp"
#include "gamelayout.hpp"
#include "rowcol.hpp"
#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

enum class Orientation { Horizontal, Vertical };
constexpr int UNSET = -1;
struct Ship {

  // Typedefs
  using Ships = std::vector<Ship>;

  struct ShipCollision {
    enum class Status { hit, miss, invalid };
    Status status;
    ShipDefinition id;
    constexpr operator bool() const { return status == Status::hit; }
  };

  // Data
  ShipDefinition shiplength;
  collision::AABB location{UNSET, UNSET, UNSET, UNSET};

  // Functions
  constexpr Orientation orientation() const noexcept {
    if ((location.x2 - location.x) > (location.y2 - location.y))
      return Orientation::Horizontal;

    return Orientation::Vertical;
  }

  constexpr int row_size() const noexcept { return location.x2 - location.x; }

  constexpr int col_size() const noexcept { return location.y2 - location.y; }

  constexpr bool is_valid() const noexcept {
    return (row_size() == 0 && col_size() == shiplength.size - 1) ||
           (row_size() == shiplength.size - 1 && col_size() == 0);
  }

  constexpr bool is_placed_valid(GameLayout const &layout) const noexcept {
    if (!is_valid())
      return false;
    if (location.x < 0 || location.x >= layout.nbrCols.size ||
        location.x2 < 0 || location.x2 >= layout.nbrCols.size ||
        location.y < 0 || location.y >= layout.nbrRows.size ||
        location.y2 < 0 || location.y2 >= layout.nbrRows.size)
      return false;

    return true;
  }

  constexpr std::optional<std::size_t> ship_section_hit(RowCol const &pos) {
    int x = static_cast<int>(pos.col.size);
    int y = static_cast<int>(pos.row.size);

    if (!location.contains_point(x, y))
      return {};

    int yIndex = y - location.y;
    int xIndex = x - location.x;

    if (yIndex == 0)
      return static_cast<std::size_t>(xIndex);
    if (xIndex == 0)
      return static_cast<std::size_t>(yIndex);

    return {};
  }

  constexpr ShipDefinition id() const noexcept { return shiplength; }

  static constexpr ShipCollision shot_at(Ships const &ships,
                                         RowCol const &shot) {
    for (auto &ship : ships) {
      if (ship.location.contains_point((int)shot.col.size,
                                       (int)shot.row.size)) {
        return {ShipCollision::Status::hit, ship.id()};
      }
    }
    return {ShipCollision::Status::miss, 0};
  }

  static constexpr bool any_collisions(Ships const &ships) {
    return collision::any_collision(ships,
                                    [](auto &ship) { return ship.location; });
  }
  static std::optional<Ship> ship_at_position(Ships const &ships, RowCol pos);
  static std::optional<Ships> random_ships(GameLayout const &layout);
};

constexpr bool ships_collide(Ship const &ship1, Ship const &ship2) {
  return collision::aabb_collision(ship1.location, ship2.location);
}

// std::optional<Ship> ship_at_position(Ships const &ships, Row r, Col c) {
//   return ship_at_position(ships, RowCol{r, c});
// }
