#pragma once

#include "rowcol.hpp"
#include <cstddef>
struct ShipDefinition {
  std::size_t size;

  auto operator<=>(ShipDefinition const &other) const = default;
  bool operator==(ShipDefinition const &other) const = default;
};

struct GameLayout {
  ShipDefinition minShipSize{2};
  ShipDefinition maxShipSize{5};
  Row nbrRows{10};
  Col nbrCols{10};

  constexpr bool is_valid() const {
    if (minShipSize.size <= 0 or maxShipSize.size >= nbrRows.size or
        maxShipSize.size >= nbrCols.size or minShipSize >= maxShipSize)
      return false;

    return true;
  }

  constexpr std::size_t shipdef_to_index(ShipDefinition ship) const {
    return ship.size - minShipSize.size;
  }

  constexpr bool is_row_col_valid(RowCol const &pos) const noexcept {
    return (pos.row >= Row(0) && pos.row < nbrRows) and
           (pos.col >= Col(0) and pos.col < nbrCols);
  }
};
