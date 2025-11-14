#pragma once

#include "image.hpp"
#include <iostream>
#include <print>
#include <type_traits>

namespace term {

namespace details {

template <class PXFormat>
  requires std::is_base_of_v<Color, PXFormat>
unsigned int extract_color(PXFormat *px) {
  unsigned int c = 0;
  c |= px->red << 16;
  c |= px->green << 8;
  c |= px->blue;
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

template <> struct WhatToCache<Color> {
  unsigned int prev_color{0};
};

template <> struct WhatToCache<BgColor> {
  unsigned int prev_backgroun{0};
};

template <class... T> struct Cache : public WhatToCache<T...> {};

template <class PXFormat> void render(PXFormat *p) {

  Cache<typename PXFormat::components::type> cache;

  for (int x = 0; x < 10; ++x) {
    if constexpr (term::Image<PXFormat>::is_color) {
      unsigned int curr = extract_color(p);
      if (cache.prev_color != curr) {
        // write_color(curr);
        std::print("\e[38;2;{};{};{}mx", curr >> 16 & 0xFF, curr >> 8 & 0xFF,
                   curr & 0xFF);
        cache.prev_color = curr;
      }
    }

    if constexpr (term::Image<PXFormat>::is_ascii::value) {
      std::cout << (char)p->value;
    }

    ++p;
  }
}

// template<class PXFormat>
// void render_to_screen( Image<PXFormat> & img ) {
// if constexpr ( typename PXFormat::is_ASCII ) {

} // namespace details
} // namespace term
