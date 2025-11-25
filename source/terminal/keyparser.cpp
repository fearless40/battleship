#include "keyparser.hpp"
#include <cctype>
#include <charconv>
#include <cstring>
#include <iostream>
#include <string_view>

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

template <typename Iterator, typename Sentinal>
void next_match(char letter, Iterator &start, const Sentinal S) {
  while (start != S) {
    if (*start == letter)
      return;
    ++start;
  }
}

KeyStatus parse(const std::string_view buffer) noexcept {

  // Key protocol looks like
  // CSI unicode-key-code:alternate-key-codes ; modifiers:event-type ;
  // text-as-codepoints u
  KeyStatus k;
  std::memset((void *)&k, 0, sizeof(KeyStatus));
  k.position = KeyPosition::pressed;

  // Verfiy buffer is big enough
  if (buffer.size() < 3)
    return {};

  auto start = buffer.begin();

  if (*start != '\e')
    return k;
  if (*++start != '[')
    return k;

  ++start;
  std::cout << std::string_view(start, buffer.end()) << '\n';

  k.key = digits(start, buffer.end());

  next_match(';', start, buffer.end());

  if (*start != ';')
    return k;

  int modifier = digits(++start, buffer.end()) - 1;

  if (modifier > 0) {

    if (modifier & 0b1)
      k.shift = true;
    if (modifier & 0b10)
      k.alt = true;
    if (modifier & 0b100)
      k.ctl = true;
    if (modifier & 0b1000)
      k.super = true;
  }

  if (*start != ':') {
    k.position = KeyPosition::pressed;

    return k;
  }

  ++start;
  switch (*start) {
  case '1':
    k.position = KeyPosition::pressed;
    break;
  case '2':
    k.position = KeyPosition::repeat;
    break;
  case '3':
    k.position = KeyPosition::released;
    break;
  };

  return k;
}
} // namespace term::kittykeyprotocol
