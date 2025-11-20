#include "term_control.hpp"
#include <cstdio>
#include <exception>
#include <iostream>
#include <termios.h>
#include <unistd.h>

namespace term {

static constexpr char ALTSCREEN_ON[] = "\e[?1049h";
static constexpr char ALTSCREEN_OFF[] = "\e[?1049h";

TermControl::TermControl() {
  // Unbuffer ouput for C++ streams
  std::ios_base::sync_with_stdio(false);
  std::cout.tie(nullptr);
  std::cout << std::unitbuf;

  // Without unitbuf the cout will not write without a flush command.
  // Not sure if better to auto flush or just force flush.
  // Possible other options include using lower level c functions such as
  // write...

  // See https://stackoverflow.com/questions/18412164/fast-c-string-output

  init_term();
}

TermControl::~TermControl() {

  std::cout << (ALTSCREEN_OFF);
  tcsetattr(STDIN_FILENO, TCSANOW, &tp_old_);
}

void TermControl::init_term() {

  termios tp;

  tcgetattr(STDIN_FILENO, &tp);

  tp_old_ = tp;

  // Enabling raw terminal input mode
  tp.c_iflag &= ~IGNBRK; // Disable ignoring break condition
  tp.c_iflag &= ~BRKINT; // Disable break causing input and output to be
                         // flushed
  tp.c_iflag &= ~PARMRK; // Disable marking parity errors.
  tp.c_iflag &= ~ISTRIP; // Disable striping 8th bit off characters.
  tp.c_iflag &= ~INLCR;  // Disable mapping NL to CR.
  tp.c_iflag &= ~IGNCR;  // Disable ignoring CR.
  tp.c_iflag &= ~ICRNL;  // Disable mapping CR to NL.
  tp.c_iflag &= ~IXON;   // Disable XON/XOFF flow control on output

  tp.c_lflag &= ~ECHO;   // Disable echoing input characters.
  tp.c_lflag &= ~ECHONL; // Disable echoing new line characters.
  tp.c_lflag &= ~ICANON; // Disable Canonical mode.
  tp.c_lflag &= ~ISIG;   // Disable sending signal when hitting:
                         // -     => DSUSP
                         // - C-Z => SUSP
                         // - C-C => INTR
                         // - C-d => QUIT
  tp.c_lflag &= ~IEXTEN; // Disable extended input processing
  tp.c_cflag |= CS8;     // 8 bits per byte

  tp.c_cc[VMIN] = 0;  // Minimum number of characters for non-canonical
                      // read.
  tp.c_cc[VTIME] = 0; // Timeout in deciseconds for non-canonical read.

  tcsetattr(STDIN_FILENO, TCSANOW, &tp);

  // Send signals to terminal that we want alternate mode rendering
  //
  //
  //
  // Enable alternate window rendering to overwrite users term
  std::cout << (ALTSCREEN_ON);
}

}; // namespace term
