#include "guessallai.hpp"
#include <optional>

std::optional<RowCol> GuessAllAi::guess() {

  if (m_col == game.nbrCols) {
    ++m_row.size;
    m_col.size = 0;
  }
  if (m_row == game.nbrRows)
    return {};

  auto col = m_col;
  ++m_col.size;
  return RowCol{m_row, col};
}
