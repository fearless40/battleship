#pragma once
#include <string_view>

namespace term::details {

constexpr char *to_chars(char *start, char *end, int value) {

  if (value > 9999 || value < 0)
    return start;

  int nbrChars = 4;

  if (value <= 9)
    nbrChars = 1;
  else if (value <= 99)
    nbrChars = 2;
  else if (value <= 999)
    nbrChars = 3;
  else if (value <= 9999)
    nbrChars = 4;

  if (nbrChars + start > end)
    return start;

  int i = nbrChars;
  do {
    start[--i] = '0' + (value % 10);
    value /= 10;
  } while (value != 0);
  return &start[nbrChars];
}
} // namespace term::details

namespace colors {

#define COLORF(r, g, b) ("\e[38;2;" #r ";" #g ";" #b "m")

constexpr const std::string_view red = COLORF(200, 0, 0);
constexpr const std::string_view green = COLORF(0, 200, 0);
constexpr const std::string_view blue = COLORF(0, 0, 200);
constexpr const std::string_view reset = "\e[0m";
constexpr const std::string_view light_gray = COLORF(190, 190, 190);
constexpr const std::string_view dark_gray = COLORF(90, 90, 90);

#undef COLORF

} // namespace colors
//

namespace move {

#define MOVE(letter) ("\e[" #letter)
constexpr const std::string_view home = MOVE(H);

constexpr const std::string position(int line, int col) {
  char command[2 + 4 + 1 + 4 + 1] = "\e[";
  char *next = &command[2];
  char *end = &command[11];
  next = term::details::to_chars(next, end, line);
  *next = ';';
  next = term::details::to_chars(next, end, col);
  *next = 'H';
  *(++next) = 0;
  return command;
}

constexpr const std::string relative(int value, char motion) {
  char command[2 + 4 + 1 + 1] = "\e[";
  char *next = &command[2];
  char *end = &command[7];
  next = term::details::to_chars(next, end, value);
  *next = motion;
  *(++next) = 0;
  return command;
}

constexpr const std::string up(int amount) { return relative(amount, 'A'); }

constexpr const std::string down(int amount) { return relative(amount, 'B'); }

constexpr const std::string right(int amount) { return relative(amount, 'C'); }

constexpr const std::string left(int amount) { return relative(amount, 'D'); }

constexpr const std::string down_front(int amount) {
  return relative(amount, 'E');
}

constexpr const std::string up_front(int amount) {
  return relative(amount, 'F');
}

constexpr const std::string column(int amount) { return relative(amount, 'G'); }
} // namespace move

namespace erase {

constexpr std::string cur_endscreen = "\e[0J";
constexpr std::string cur_begscreen = "\e[1J";
constexpr std::string all = "\e[2J";
constexpr std::string cur_endline = "\e[0K";
constexpr std::string cur_begline = "\e[1K";
constexpr std::string line = "\e[2K";
} // namespace erase
