#pragma once
#include "typed_scalar.hpp"
#include <cstdint>
#include <tuple>

namespace geom {

template <typename... Channels> struct ColorG {
  using tupple_t = std::tuple<Channels...>;
  using type = ColorG<Channels...>;

  tupple_t channels;

  ColorG(Channels... args) : channels(args...) {}

  template <class ChannelType> constexpr auto &get_channel() noexcept {
    return std::get<ChannelType>(channels);
  }
  template <class ChannelType>
  constexpr const auto get_channel() const noexcept {
    return std::get<ChannelType>(channels);
  }

  static constexpr type blend(const type &lhs, const type &rhs) {

    return type{
        lhs.get_channel<Channels>() + rhs.get_channel<Channels>()...,
    };
  }
};

namespace detail {
struct Red {};
struct Green {};
struct Blue {};
struct Hue {};
struct Saturaton {};
struct Luminance {};

using RedByte = TypedScalar<std::uint8_t, Red>;

using GreenByte = TypedScalar<std::uint8_t, Green>;
using BlueByte = TypedScalar<std::uint8_t, Blue>;
} // namespace detail

struct RGB : ColorG<detail::RedByte, detail::GreenByte, detail::BlueByte> {
  using ColorG::ColorG;

  constexpr detail::RedByte &red() noexcept {
    return get_channel<detail::RedByte>();
  }

  constexpr detail::GreenByte &green() noexcept {
    return get_channel<detail::GreenByte>();
  }

  constexpr detail::BlueByte &blue() noexcept {
    return get_channel<detail::BlueByte>();
  }
};

}
template <typename C1, typename C2, typename C3> struct Color3 {

  C1 c1;
  C2 c2;
  C3 c3;
};

} // namespace geom
