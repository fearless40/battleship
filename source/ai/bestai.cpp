#include "bestai.hpp"
#include "gamelayout.hpp"
#include <algorithm>
#include <array>
#include <iterator>
#include <random>

std::random_device rd2("default");

void BestAI::new_game(const GameLayout &game_input) {
  game = game_input;
  m_guess.resize(game_input.nbrRows, game_input.nbrCols);
  m_guess.fill(Status::unknown);
  m_prob.resize(game_input.nbrRows, game_input.nbrCols);
  m_prob.fill(0);
  m_sunk.clear();
}

RowCol BestAI::set_guess(RowCol g) {
  last_guess = g;
  ++m_nbr_guess;
  return last_guess;
}

void BestAI::clear_probability() noexcept { m_prob.fill(0); }

std::vector<int> BestAI::ships_still_alive() const {

  std::vector<int> sunk_ships;

  std::transform(m_sunk.begin(), m_sunk.end(), std::back_inserter(sunk_ships),
                 [](auto &value) { return value.size; });

  std::sort(sunk_ships.begin(), sunk_ships.end());

  std::vector<int> all_ships((game.maxShipSize.size - game.minShipSize.size) +
                             1);
  std::vector<int> diff;

  std::iota(all_ships.begin(), all_ships.end(), game.minShipSize.size);

  std::set_difference(all_ships.begin(), all_ships.end(), sunk_ships.begin(),
                      sunk_ships.end(), std::back_inserter(diff));

  return diff;
}

int BestAI::get_weight_for_status(Status status) const noexcept {
  switch (status) {
  case Status::unknown:
    return 1;
  case Status::empty:
    return -1;
  case Status::hit:
    return -1;
  default:
    return 0;
  }
}

void BestAI::test_cols_if_ship_fits(Row row, Col start_col,
                                    ShipDefinition ship) {

  if (start_col + Col(ship.size) > game.nbrCols)
    return;

  auto endCol = std::min(Col(start_col) + Col(ship.size), game.nbrCols);

  for (auto c_hits = start_col; c_hits < endCol; ++c_hits) {

    if (m_guess[RowCol(row, c_hits)] != Status::unknown) {
      return;
    }
  }

  for (auto c_hits = start_col; c_hits < endCol; ++c_hits) {
    m_prob[RowCol(row, c_hits)] += 1;
  }
}

void BestAI::test_rows_if_ship_fits(Col col, Row start_row,
                                    ShipDefinition ship) {
  if (start_row + Row(ship.size) > game.nbrRows)
    return;

  auto endRow = std::min(Row(start_row) + Row(ship.size), game.nbrRows);

  for (auto r_hits = start_row; r_hits < endRow; ++r_hits) {
    if (m_guess[RowCol(r_hits, col)] != Status::unknown)
      return;
  }

  for (auto r_hits = start_row; r_hits < endRow; ++r_hits) {
    m_prob[RowCol(r_hits, col)] += 1;
  }
}

void BestAI::generate_probability() noexcept {
  clear_probability();
  auto ships_left = ships_still_alive();

  for (auto row = Row(0); row < game.nbrRows; ++row) {
    for (auto col = Col(0); col < game.nbrCols; ++col) {

      for (auto ship_sz : ships_left) {
        test_cols_if_ship_fits(row, col, ShipDefinition(ship_sz));
        test_rows_if_ship_fits(col, row, ShipDefinition(ship_sz));
      }
      if (m_guess[RowCol(row, col)] == Status::hit) {
        std::array<RowCol, 4> positions{
            RowCol(row - Row(1), col), RowCol(row + Row(1), col),
            RowCol(row, col - Col(1)), RowCol(row, col + Col(1))};

        // Add extra weights for positions that have a ship that is hit.
        for (auto pos : positions) {
          if (m_prob.is_valid_index(pos)) {
            if (m_guess[pos] == Status::unknown)

              m_prob[pos] += /*game.max.size +*/ 5;
          }
        }
      }
    }
  }
}

RowCol BestAI::get_highest_prob_of_ship() const noexcept {

  auto max_location = RowCol(Row(0), Col(0));
  int max_value = 0;
  for (auto r = Row(0); r < game.nbrRows; ++r) {
    for (auto c = Col(0); c < game.nbrCols; ++c) {
      if (max_value < m_prob[RowCol(r, c)]) {
        max_value = m_prob[RowCol(r, c)];
        max_location = RowCol(r, c);
      }
    }
  }
  return max_location;
}

std::optional<RowCol> BestAI::guess() {
  generate_probability();
  auto guess = get_highest_prob_of_ship();

  // for (auto r = Row(0); r < game.rows; ++r) {
  //   for (auto c = Col(0); c < game.cols; ++c) {
  //     std::cout << m_prob[RowCol(r, c)] << "   ";
  //   }
  //   std::cout << '\n';
  // }
  //
  return set_guess(guess);

  // Check if we have any pre-generated guess we need to make and return

  // if (m_next_guesses.size() > 0) {
  //   auto guess = m_next_guesses.back();
  //   m_next_guesses.pop_back();
  //   return set_guess(guess);
  // }
  //
  // Make a random guess
  auto rd_row =
      std::uniform_int_distribution<unsigned short>(0, game.nbrRows.size - 1);
  auto rd_col =
      std::uniform_int_distribution<unsigned short>(0, game.nbrCols.size - 1);

  // Pick a random point and then search for next guess.
  auto pos = RowCol{Row{rd_row(rd2)}, Col{rd_col(rd2)}};
  if (auto next_guess = m_guess.find_index_from(Status::unknown, pos);
      next_guess) {
    return set_guess(next_guess.value());
  } else {
    return {};
  }
}

void BestAI::miss() {
  // Set the area as a miss
  m_guess.set(last_guess, Status::empty);
}

void BestAI::hit() {
  // Calculate a grid of values to try and add them to the list
  m_guess.set(last_guess, Status::hit);

  // auto up = last_guess - battleship::Row{1};
  // auto down = last_guess + battleship::Row{1};
  // auto left = last_guess - battleship::Col{1};
  // auto right = last_guess + battleship::Col{1};
  //
  // if (m_guess.is_valid_index(up) && m_guess[up] == Status::unknown)
  //   m_next_guesses.push_back(up);
  //
  // if (m_guess.is_valid_index(down) && m_guess[down] == Status::unknown)
  //   m_next_guesses.push_back(down);
  //
  // if (m_guess.is_valid_index(left) && m_guess[left] == Status::unknown)
  //   m_next_guesses.push_back(left);
  // if (m_guess.is_valid_index(right) && m_guess[right] == Status::unknown)
  //   m_next_guesses.push_back(right);
}

void BestAI::sink(ShipDefinition id) {
  m_sunk.push_back(id);
  m_guess.set(last_guess, Status::hit);
}
