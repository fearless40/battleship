#pragma once

#include "image.hpp"
#include <span>
#include <type_traits>
#include <utility>

namespace term {

namespace details {

template <class PXFormat>
  requires std::is_base_of_v<pixel::Color, PXFormat>
unsigned int extract_color(PXFormat &px) {
  unsigned int c = 0;
  c |= px.red << 16;
  c |= px.green << 8;
  c |= px.blue;
  return c;
}
template <class PXFormat> unsigned int extract_bgcolor(PXFormat *px) {
  unsigned int c = 0;
  c |= px->bgred << 16;
  c |= px->bggreen << 8;
  c |= px->bgblue;
  return c;
}

template <class T> struct WhatToCache {};

template <> struct WhatToCache<pixel::Color> {
  unsigned int prev_color{0};
};

template <> struct WhatToCache<pixel::BgColor> {
  unsigned int prev_backgroun{0};
};

template <class... T> struct Cache : public WhatToCache<T>... {};

template <typename T>
using Make_Cache = decltype(std::apply(
    [](auto... t) consteval { return Cache<decltype(t)...>{}; },
    std::declval<T>()));

template <typename CPtr> constexpr CPtr to_chars(CPtr start, int value) {

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

  int i = nbrChars;
  do {
    start[--i] = '0' + (value % 10);
    value /= 10;
  } while (value != 0);
  return start + nbrChars;
}

template <typename CPtr>
constexpr CPtr relative(CPtr begin, int value, char motion) {
  CPtr c = begin;
  *c = '\e';
  *++c = '[';
  c = to_chars(++c, value);
  *++c = motion;
  return ++c;
}

template <typename CPtr>
constexpr CPtr move_down_relative(CPtr begin, int amount = 1) {
  return relative(begin, amount, 'B');
}

template <typename CPtr>
constexpr CPtr move_left_relative(CPtr begin, int amount = 1) {
  return relative(begin, amount, 'D');
}

using OutputBuffer = std::span<unsigned char>;

constexpr void write_color(typename OutputBuffer::iterator &out, int color) {
  *out = '\e';
  *++out = '[';
  *++out = '3';
  *++out = '8';
  *++out = ';';
  *++out = '2';
  *++out = ';';
  out = to_chars(++out, (color >> 16) & 0xFF);
  *out = ';';
  out = to_chars(++out, (color >> 8) & 0xFF);
  *out = ';';
  out = to_chars(++out, color & 0xFF);
  *out = 'm';
  ++out;
}

template <class PXFormat>
void render_to_buffer(const Image<PXFormat> &img,
                      std::span<unsigned char> buffer) {

  Make_Cache<typename PXFormat::components> cache{};

  auto out = buffer.begin();

  out = relative(out, 20, 'A');
  out = relative(out, 30, 'd');

  for (unsigned int x = 0; x < img.width(); ++x) {
    for (unsigned int y = 0; y < img.height(); ++y) {
      auto &p = img.pixel(x, y);

      if constexpr (pixel::PixelFormatDetails<PXFormat>::is_color) {
        unsigned int curr = extract_color(p);
        if (cache.prev_color != curr) {
          write_color(out, curr);
          cache.prev_color = curr;
        }
      }

      if constexpr (pixel::PixelFormatDetails<PXFormat>::is_ascii) {
        *out = p.value;
        ++out;
      }
    }
    // Move to the next line and move cursor to where we where before...
    // out = move_down_relative(out, 1);
    *out = '\n';
    ++out;
    // out = move_left_relative(out, img.width());
  }
  *++out = 0;
}

// template<class PXFormat>
// void render_to_screen( Image<PXFormat> & img ) {
// if constexpr ( typename PXFormat::is_ASCII ) {

} // namespace details
} // namespace term
