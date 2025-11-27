#pragma once

#include "keyparser.hpp"
#include <termios.h>

namespace term {
class TermControl {
private:
  void init_term();
  void reset_term();
  void get_width_height();
  void install_signals();
  void uninstall_signals();
  termios tp_old_;

  input::KeyStatus keyevt_;
  bool key_changed_{false};
  input::MouseStatus mouseevt_;
  bool mouse_changed_{false};

  bool window_resized_{false};

  int width_{0};
  int height_{0};
  int px_width_{0};
  int px_height_{0};

public:
  TermControl();
  ~TermControl();
  void on_loop();

  constexpr bool had_mouse_event() const noexcept { return mouse_changed_; };
  constexpr bool had_key_event() const noexcept { return key_changed_; };
  constexpr const input::KeyStatus &get_key_event() const noexcept {
    return keyevt_;
  }
  constexpr const input::MouseStatus &get_mouse_event() const noexcept {
    return mouseevt_;
  }

  constexpr int width() const noexcept { return width_; }
  constexpr int height() const noexcept { return height_; }

  constexpr int px_width() const noexcept { return px_width_; }
  constexpr int px_height() const noexcept { return px_height_; }
};
} // namespace term
