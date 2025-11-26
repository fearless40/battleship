#include "term_control.hpp"
#include "keyparser.hpp"
#include <cctype>
#include <charconv>
#include <cstddef>
#include <cstdio>
#include <exception>
#include <ios>
#include <iostream>
#include <poll.h>
#include <print>
#include <string_view>
#include <termios.h>
#include <unistd.h>

namespace term {

namespace codes {
static constexpr char ALTSCREEN_ON[] = "\e[?1049h";
static constexpr char ALTSCREEN_OFF[] = "\e[?1049l";
static constexpr char CSI[] = "\x9B";
static constexpr char DCS[] = "\x90";
static constexpr char OSC[] = "\x9D";

#define CSI "\e["

static constexpr char KITTYKEY_CODE_PROTOCOL_ON[] = CSI ">1u";
static constexpr char KITTYKEY_CODE_PROTOCOL_OFF[] = CSI "<u";
static constexpr char KITTYKEY_ENHACEMENT[] =
    CSI "=11u"; // Disambiguate escape codes, report event types, report all
                // keys as escape codes
static constexpr char VT200_MOUSE_ON[] = CSI "?1000h";
static constexpr char MOUSE_BUTTON_ALL_EVENTS_ON[] = CSI "?1002h";
static constexpr char MOUSE_MOVEMENT_EVENTS_ON[] = CSI "?1003h";
static constexpr char SGR_MOUSE_ON[] = CSI "?1006h";
#undef CSI
} // namespace codes

TermControl::TermControl() {
  // Unbuffer ouput for C++ streams
  std::ios_base::sync_with_stdio(false);
  std::cout.tie(nullptr);
  std::unitbuf(std::cout);
  std::cin.tie(nullptr);
  std::unitbuf(std::cin);

  // Without unitbuf the cout will not write without a flush command.
  // Not sure if better to auto flush or just force flush.
  // Possible other options include using lower level c functions such as
  // write...

  // See https://stackoverflow.com/questions/18412164/fast-c-string-output

  init_term();
}

TermControl::~TermControl() {

  // Turn off alternative screen;
  reset_term();

  std::ios_base::sync_with_stdio(true);
  // std::cout.tie(&std::cin);
  // std::cout << std::unitbuf;
}

void TermControl::reset_term() {

  std::cout << codes::KITTYKEY_CODE_PROTOCOL_OFF;
  std::cout << (codes::ALTSCREEN_OFF);
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
  tp.c_iflag &= ~PARMRK; // Disable marking parity errorsCSI
                         // unicode-key-code:alternate-key-codes ;
                         // modifiers:event-type ; text-as-codepoints u.
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

  // Enable alternate window rendering
  std::cout << (codes::ALTSCREEN_ON);

  // Set Kitty text processing
  // clang-format off
   std::cout
      << codes::KITTYKEY_CODE_PROTOCOL_ON
      << codes::KITTYKEY_ENHACEMENT
      << codes::VT200_MOUSE_ON
      << codes::MOUSE_BUTTON_ALL_EVENTS_ON
      << codes::MOUSE_MOVEMENT_EVENTS_ON
      << codes::SGR_MOUSE_ON;

  // clang-format on
}

void TermControl::on_loop() {
  pollfd pdata;

  pdata.fd = STDIN_FILENO;
  pdata.events = POLLIN;

  if (auto ret = poll(&pdata, 1, 0); ret == -1) {
    // Some error occurded terminate!
    std::cout << "Error: " << ret;
    std::terminate();
  }

  if (pdata.revents & POLLIN) {
    // Read the data from stdin
    char buff[128];
    auto amount = read(STDIN_FILENO, buff, 128);
    if (amount <= 0)
      return;

    const std::string_view buffer_view(buff, amount);
    std::cout << buffer_view << '\n';
    if (input::is_mouse_protocol(buffer_view)) {
      auto ms = input::parse_mouse(buffer_view);
      // std::println("Mouse {}:{} Button {} ", ms.row, ms.col, (int)ms.button);

    } else {
      auto key = input::parse_key(buffer_view);
      // std::println("Key {} Shift {} Ctrl {} Alt {} Pressed {} Released {}",
      //              key.key, (bool)key.shift, (bool)key.ctl, (bool)key.alt,
      //              key.position == input::KeyPosition::pressed,
      //              key.position == input::KeyPosition::released);
    }
  }
}

}; // namespace term
