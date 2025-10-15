#pragma once
#include <concepts>
#include <string>
#include <type_traits>

namespace base26 {
constexpr int from_string(std::string_view const letters) {
  int acc = 0;
  for (char c : letters) {
    acc = acc * 26 + (c - 'a');
  }
  return acc;
}

namespace detail {
inline constexpr char charconv_digits[] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
}

template <std::integral T>
constexpr std::string_view to_string(T value, char *begin_buf, char *end_buf) {
  using unsigned_type = std::make_unsigned_t<T>;
  unsigned_type val = static_cast<unsigned_type>(value);
  char *end = end_buf;
  char *first = begin_buf;

  if (begin_buf == end_buf) {
    return {begin_buf, 0};
  }

  if constexpr (std::is_signed_v<T>) {
    if (value < 0) {
      *first++ = '-';

      val = static_cast<unsigned_type>(0 - value);
    }
  }

  do {
    *--end = 'a' + val % 26; // charconv_digits[val % 26];
    val = val / 26;
  } while (val != 0);

  while (end != end_buf) {
    *first++ = *end++;
  }

  return {begin_buf, static_cast<size_t>(first - begin_buf)};
}

constexpr std::string to_string(int value) {
  char buffer[16]{"              "};
  return std::string{to_string(value, (char *)buffer, (char *)&buffer[15])};
}

/*static_assert(to_string(0) == "a");
static_assert(to_string(1) == "b");
static_assert(to_string(27) == "bb");
static_assert(to_string(-1) == "-b");*/

} // namespace base26

namespace base10 {
constexpr int from_string(std::string_view const letters) {
  int acc = 0;
  for (char c : letters) {
    acc = acc * 10 + (c - '0');
  }
  return acc;
}

template <std::integral T>
constexpr std::string_view to_string(T value, char *begin_buf, char *end_buf) {
  using unsigned_type = std::make_unsigned_t<T>;
  unsigned_type val = static_cast<unsigned_type>(value);
  char *end = end_buf;
  char *first = begin_buf;

  if (begin_buf == end_buf) {
    return {begin_buf, 0};
  }

  if constexpr (std::is_signed_v<T>) {
    if (value < 0) {
      *first++ = '-';
      val = static_cast<unsigned_type>(0 - value);
    }
  }

  do {
    *--end = '0' + val % 10; // charconv_digits[val % 26];
    val /= 10;
  } while (val != 0);

  // Copy to front
  while (end != end_buf) {
    *first++ = *end++;
  }

  return {begin_buf, static_cast<size_t>(first - begin_buf)};
}

constexpr std::string_view to_string(int value) {
  char buffer[16];
  return to_string(value, buffer, &buffer[15]);
}

/*static_assert(to_string(0) == "0");
static_assert(to_string(1) == "1");
static_assert(to_string(27) == "27");
static_assert(to_string(-1) == "-1");*/
} // namespace base10
