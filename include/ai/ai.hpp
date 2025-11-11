#pragma once
#include "gamelayout.hpp"
#include <optional>

class AI {
public:
  virtual ~AI() {};
  virtual void new_game(const GameLayout &game) = 0;
  virtual void miss() = 0;
  virtual void hit() = 0;
  virtual void sink(ShipDefinition shipId) = 0;

  // Return Empty guess to indicate no more guesses
  virtual std::optional<RowCol> guess() = 0;
  virtual const std::string_view description() const = 0;
};
