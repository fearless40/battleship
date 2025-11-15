#pragma once

#include <memory>
#include <tuple>
#include <type_traits>

namespace term {

using byte = unsigned char;

struct alignas(1) Color {
  byte red;
  byte green;
  byte blue;
};
struct alignas(1) BgColor {
  byte bgred;
  byte bggreen;
  byte bgblue;
};

struct alignas(1) FormatFlags {
  byte underline : 1;   // 3
  byte dbunderline : 1; // 4
  byte bold : 1;        // 6
  byte italic : 1;      // 7
};

struct alignas(1) ASCII {
  unsigned char value;
};

struct alignas(2) UTF16 {
  unsigned short value;
  unsigned short lookup;
};

struct alignas(1) UTF8 {
  unsigned char value;
  unsigned char lookup; // If the character is actually larger...
};

struct alignas(4) UTF32 {
  unsigned int value;
  unsigned int lookup;
};

struct alignas(1) DoubleBlock {
  unsigned char value; // Between 0 - 3 inclusive
};

struct alignas(1) QuadrantBlock {
  unsigned char value; // Between 0 - 6 inclusive
};

namespace details {
template <class... Components> struct alignas(4) Pixel_ : public Components... {
  using components = std::tuple<Components...>;
  using type = Pixel_<Components...>;
};

} // namespace details

using Pixel_ASCII = details::Pixel_<Color, BgColor, ASCII>;
using Pixel_Full = details::Pixel_<FormatFlags, Color, BgColor, ASCII>;

template <typename PixelType> struct PixelFormatDetails {
  static constexpr auto is_ascii = std::is_base_of_v<ASCII, PixelType>;
  static constexpr auto is_utf8 = std::is_base_of_v<UTF8, PixelType>;
  static constexpr auto is_bgcolor = std::is_base_of_v<BgColor, PixelType>;
  static constexpr auto is_color = std::is_base_of_v<Color, PixelType>;
  static constexpr auto is_termstyle =
      std::is_base_of_v<FormatFlags, PixelType>;
};

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
