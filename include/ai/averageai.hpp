#pragma once

#include "ai.hpp"
#include "array2d.hpp"
#include "gamelayout.hpp"
#include "rowcol.hpp"
#include <string_view>
#include <vector>

class AverageAI : public AI {
private:
  enum class Status { unknown, empty, hit };
  GameLayout game;
  RowCol last_guess;
  Array2D<Status> m_guess;
  std::vector<RowCol> m_next_guesses;
  std::size_t m_nbr_guess{0};
  std::vector<ShipDefinition> m_sunk;

private: // Methods
  RowCol set_guess(RowCol g);

public:
  ~AverageAI() {}

  void new_game(const GameLayout &game_input) override;
  void hit() override;
  void miss() override;
  void sink(ShipDefinition shipId) override;
  std::optional<RowCol> guess() override;

  const std::string_view description() const override;
};
