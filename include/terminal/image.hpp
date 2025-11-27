#pragma once

#include "pixel.hpp"
#include <memory>
#include <type_traits>

namespace term {

using Pixel_ASCII = pixel::Pixel<pixel::Color, pixel::BgColor, pixel::ASCII>;
using Pixel_Full = pixel::Pixel<pixel::FormatFlags, pixel::Color,
                                pixel::BgColor, pixel::ASCII>;

template <class PixelFormat> class Image {
private:
  unsigned int width_;
  unsigned int height_;
  std::unique_ptr<PixelFormat[]> buffer;

  constexpr PixelFormat *pixel_at(unsigned int x, unsigned int y) {
    return &buffer[x * height_ + y];
  }

  constexpr const PixelFormat *pixel_at(unsigned int x, unsigned int y) const {
    return &buffer[x * height_ + y];
  }

public:
  using PXFormat = PixelFormat;
  Image(unsigned int width, unsigned int height)
      : width_(width), height_(height),
        buffer(new PixelFormat[width_ * height_]) {};

  constexpr unsigned int width() const noexcept { return width_; }
  constexpr unsigned int height() const noexcept { return height_; }

  constexpr void set_pixel(unsigned int x, unsigned int y, PixelFormat &p) {
    *pixel_at(x, y) = p;
  }

  constexpr const PixelFormat &pixel(unsigned int x,
                                     unsigned int y) const noexcept {
    return *pixel_at(x, y);
  }
};

// static_assert(std::is_base_of_v<Utf16, FullPixel>);
// static_assert(std::is_base_of_v<ASCII, FullPixel>);

} // namespace term
