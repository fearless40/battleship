#pragma once

#include <termios.h>
namespace term {
class TermControl {
private:
  void init_term();
  void reset_term();
  termios tp_old_;

public:
  TermControl();
  ~TermControl();
  void on_loop();
};
} // namespace term
