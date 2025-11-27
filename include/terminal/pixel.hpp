#pragma once

#include <tuple>
namespace term::pixel {

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
  byte value;
};

struct alignas(2) UTF16 {
  unsigned short value;
  unsigned short lookup;
};

struct alignas(2) UTF8 {
  byte value;
  unsigned short lookup; // If the character is actually larger...
};

struct alignas(4) UTF32 {
  unsigned int value;
  unsigned int lookup;
};

struct alignas(1) DoubleBlock {
  byte value; // Between 0 - 3 inclusive
};

struct alignas(1) QuadrantBlock {
  byte value; // Between 0 - 6 inclusive
};

struct alignas(1) Transparent {
  byte is_transparent;
};

template <class... Components> struct alignas(4) Pixel : public Components... {
  using components = std::tuple<Components...>;
  using type = Pixel<Components...>;
};

template <typename PixelType> struct PixelFormatDetails {
  static constexpr auto is_ascii = std::is_base_of_v<ASCII, PixelType>;
  static constexpr auto is_utf8 = std::is_base_of_v<UTF8, PixelType>;
  static constexpr auto is_bgcolor =
      std::is_base_of_v<pixel::BgColor, PixelType>;
  static constexpr auto is_color = std::is_base_of_v<Color, PixelType>;
  static constexpr auto is_termstyle =
      std::is_base_of_v<pixel::FormatFlags, PixelType>;
};

} // namespace term::pixel
