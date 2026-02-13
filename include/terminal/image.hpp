#pragma once

#include "compositor_shared.hpp"
#include "pixel.hpp"
#include <cstddef>
#include <memory>
#include <ranges>
#include <type_traits>

namespace term {

using Pixel_ASCII = pixel::Pixel<pixel::Color, pixel::BgColor, pixel::ASCII>;
using Pixel_Full = pixel::Pixel<pixel::FormatFlags, pixel::Color,
                                pixel::BgColor, pixel::ASCII>;

template <class PixelFormat> class Image {
public:
  using X = compositor::X;
  using Y = compositor::Y;
  using Width = compositor::Width;
  using Height = compositor::Height;

private:
  Width width_;
  Height height_;
  std::unique_ptr<PixelFormat[]> buffer;

  constexpr PixelFormat *pixel_at(X x, Y y) {
    return &buffer[y.underlying() * width_.underlying() + x.underlying()];
  }

  constexpr const PixelFormat *pixel_at(X x, Y y) const {
    return &buffer[y.underlying() * width_.underlying() + x.underlying()];
  }

public:
  using PXFormat = PixelFormat;
  Image(compositor::Width width, compositor::Height height)
      : width_(width), height_(height),
        buffer(new PixelFormat[width_ * height_]) {};

  constexpr compositor::Width width() const noexcept { return width_; }
  constexpr compositor::Height height() const noexcept { return height_; }
  static constexpr std::size_t pixel_byte_size() noexcept {
    return sizeof(PixelFormat);
  }
  constexpr std::size_t row_byte_size() const noexcept {
    return pixel_byte_size() * width_.underlying();
  }

  constexpr std::span<PixelFormat> row(compositor::Y row_) {
    return {pixel_at(compositor::X{0}, row_),
            static_cast<std::size_t>(width_.underlying())};
  }

  constexpr auto rows() {
    return std::views::iota(0, height_.underlying()) |
           std::views::transform([this](auto index) { return row(Y{index}); });
  }

  constexpr void set_pixel(X x, Y y, const PixelFormat &p) {
    *pixel_at(x, y) = p;
  }

  constexpr const PixelFormat &pixel(X x, Y y) const noexcept {
    return *pixel_at(x, y);
  }
};

// static_assert(std::is_base_of_v<Utf16, FullPixel>);
// static_assert(std::is_base_of_v<ASCII, FullPixel>);

} // namespace term
