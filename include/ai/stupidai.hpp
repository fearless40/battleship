#pragma once

#include "ai.hpp"
#include "gamelayout.hpp"
#include "rowcol.hpp"
#include "ship.hpp"

class StupidAI : public AI {
private:
  GameLayout game;
  RowCol last_guess;

public:
  ~StupidAI() {}
  StupidAI() {};

  void new_game(const GameLayout &game_input) override { game = game_input; }

  void hit() override {

  };

  void miss() override {

  };

  void sink(ShipDefinition shipId) override {

  };

  std::optional<RowCol> guess() override;

  const std::string_view description() const override {
    return "A very stupid guesser that does not check if it has guessed the "
           "same guess. Does not react to any game events. Will guess for "
           "eternity. ";
  }
};
