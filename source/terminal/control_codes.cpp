#

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
constexpr const std::string home = MOVE(H);

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
