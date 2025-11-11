#pragma once

#include "ai.hpp"
#include "array2d.hpp"
#include "gamelayout.hpp"
#include "rowcol.hpp"
#include <string_view>
#include <vector>

class BestAI : public AI {
private:
  enum class Status { unknown, empty, hit };
  GameLayout game;
  RowCol last_guess;
  Array2D<Status> m_guess;
  Array2D<int> m_prob; // Probality of spot having a ship
  std::size_t m_nbr_guess{0};
  std::vector<ShipDefinition> m_sunk;

private: // Methods
  int get_weight_for_status(Status status) const noexcept;
  RowCol set_guess(RowCol g);
  std::vector<int> ships_still_alive() const;
  void clear_probability() noexcept;
  RowCol get_highest_prob_of_ship() const noexcept;
  void generate_probability() noexcept;

  void test_cols_if_ship_fits(Row row, Col start_col, ShipDefinition ship);
  void test_rows_if_ship_fits(Col col, Row start_row, ShipDefinition ship);

public:
  ~BestAI() {}

  void new_game(const GameLayout &game_input) override;
  void hit() override;
  void miss() override;
  void sink(ShipDefinition shipId) override;
  std::optional<RowCol> guess() override;

  const std::string_view description() const override {
    return "Guess using statistics, based on best algo.";
  }
};
