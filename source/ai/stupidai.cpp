#include "stupidai.hpp"

std::optional<RowCol> StupidAI::guess() {
  return RowCol::random(Row(game.nbrRows.size - 1), Col(game.nbrCols.size - 1));
}
