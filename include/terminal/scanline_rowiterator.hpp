#pragma once

#include "compositor_shared.hpp"
#include <stack>
#include <vector>

void blah() {

  ScanLineRender sr(compoistor);
  for (auto &row : sr) {
    for (auto &col : row) {
      draw(*col);
    }
  }
}
