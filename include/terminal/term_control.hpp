#pragma once

#include <termios.h>
namespace term {
class TermControl {
private:
  void init_term();
  termios tp_old_;

public:
  TermControl();
  ~TermControl();
};
} // namespace term
