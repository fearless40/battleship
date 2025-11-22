#include "keyparser.hpp"
#include <cctype>
#include <charconv>

namespace term::kittykeyprotocol {

template <typename Iterator, typename Sentinal>
int digits(Iterator &it, const Sentinal S) {
  auto start = it;
  while (isdigit(*it) && it != S) {
    ++it;
  };

  int value{-1};
  std::from_chars(&*start, &*it, value);
  return value;
}

KeyStatus parse(const std::string_view buffer) noexcept {

  // Key protocol looks like
  // CSI key-code; modifier:number
  KeyStatus k;
  k.key = 0;

  // Verfiy buffer is big enough
  if (buffer.size() < 3)
    return {};

  auto start = buffer.begin();

  if (*start != '\e')
    return k;
  if (*++start != '[')
    return k;

  k.key = digits(start, buffer.end());

  if (*start != ';')
    return k;

  int modifier = digits(++start, buffer.end());

  // Applie modifiers ....

  if (*start != ':')
    return k;

  ++start;
  switch (*start) {
  case '1':
  case '2':
    k.position = KeyPosition::pressed;
  case '3':
    k.position = KeyPosition::released;
  };

  return k;
}
} // namespace term::kittykeyprotocol
