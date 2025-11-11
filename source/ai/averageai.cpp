#include "averageai.hpp"
#include "gamelayout.hpp"
#include "rowcol.hpp"
#include <optional>
#include <random>
std::random_device rd("default");

void AverageAI::new_game(const GameLayout &game_input) {
  game = game_input;
  m_guess.resize(game_input.nbrRows, game_input.nbrCols);
  m_guess.fill(Status::unknown);
  m_next_guesses.clear();
  m_sunk.clear();
}

RowCol AverageAI::set_guess(RowCol g) {
  last_guess = g;
  ++m_nbr_guess;
  return last_guess;
}

std::optional<RowCol> AverageAI::guess() {
  // Check if we have any pre-generated guess we need to make and return

  if (m_next_guesses.size() > 0) {
    auto guess = m_next_guesses.back();
    m_next_guesses.pop_back();
    return set_guess(guess);
  }

  // Make a random guess
  auto rd_row =
      std::uniform_int_distribution<unsigned short>(0, game.nbrRows.size - 1);
  auto rd_col =
      std::uniform_int_distribution<unsigned short>(0, game.nbrCols.size - 1);

  // Pick a random point and then search for next guess.
  auto pos = RowCol{Row{rd_row(rd)}, Col{rd_col(rd)}};
  if (auto next_guess = m_guess.find_index_from(Status::unknown, pos);
      next_guess) {
    return set_guess(next_guess.value());
  } else {
    return {};
  }
}

void AverageAI::miss() {
  // Set the area as a miss
  m_guess.set(last_guess, Status::empty);
}

void AverageAI::hit() {
  // Calculate a grid of values to try and add them to the list
  m_guess.set(last_guess, Status::hit);

  auto up = last_guess - Row{1};
  auto down = last_guess + Row{1};
  auto left = last_guess - Col{1};
  auto right = last_guess + Col{1};

  if (m_guess.is_valid_index(up) && m_guess[up] == Status::unknown)
    m_next_guesses.push_back(up);

  if (m_guess.is_valid_index(down) && m_guess[down] == Status::unknown)
    m_next_guesses.push_back(down);

  if (m_guess.is_valid_index(left) && m_guess[left] == Status::unknown)
    m_next_guesses.push_back(left);
  if (m_guess.is_valid_index(right) && m_guess[right] == Status::unknown)
    m_next_guesses.push_back(right);
}

void AverageAI::sink(ShipDefinition id) {
  m_sunk.push_back(id);
  m_guess.set(last_guess, Status::hit);
}
const std::string_view AverageAI::description() const {
  return "Guess randomly, when hitting a ship guess around the ship.";
}
