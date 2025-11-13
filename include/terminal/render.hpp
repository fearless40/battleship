#pragma once

#include "image.hpp"
#include <type_traits>

namespace terminal {
namespace details {

template <class PXFormat>
  requires std::is_base_of<Color, PXFormat>
Color extract_color(PXFormat *px) {
  return Color
}

// template<class PXFormat>
// void render_to_screen( Image<PXFormat> & img ) {
// if constexpr ( typename PXFormat::is_ASCII ) {

}
}
}
