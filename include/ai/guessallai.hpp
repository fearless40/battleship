#pragma once

#include "ai.hpp"
#include "gamelayout.hpp"
#include "rowcol.hpp"

class GuessAllAi : public AI {
private:
  GameLayout game;
  RowCol last_guess;
  Row m_row{0};
  Col m_col{0};

public:
  ~GuessAllAi() {}

  void new_game(const GameLayout &game_input) override {
    game = game_input;
    m_row = Row(0);
    m_col = Col(0);
  }

  void hit() override {

  };

  void miss() override {

  };

  void sink(ShipDefinition shipId) override {

  };

  std::optional<RowCol> guess() override;

  const std::string_view description() const override {
    return "Guess through the entire game starting at Row 0 Col 0. Very "
           "silly. ";
  }
};
