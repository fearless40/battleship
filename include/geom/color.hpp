#pragma once
#include "typed_scalar.hpp"
#include <cstdint>
#include <tuple>

namespace geom {

template <typename UnderlyingT, std::size_t NumberOfChannels>
struct ColorGeneric {
  std::array<UnderlyingT, NumberOfChannels> channels;

  constexpr UnderlyingT &operator[](std::size_t channelid) {
    return channels[channelid];
  }
  constexpr UnderlyingT operator[](std::size_t channelid) const {
    return channels[channelid];
  }
};

template <typename UnderlyingT> struct RGB : ColorGeneric<UnderlyingT, 3> {

  constexpr RGB() { this->channels.fill(static_cast<UnderlyingT>(0)); };
  constexpr RGB(UnderlyingT r, UnderlyingT g, UnderlyingT b) {
    red() = r;
    green() = g;
    blue() = b;
  }

  UnderlyingT &red() { return this->channels[0]; };

  UnderlyingT &green() { return this->channels[1]; };
  UnderlyingT &blue() { return this->channels[2]; };
};

// template <typename UnderlyingT> struct HSL : ColorGeneric<UnderlyingT, 3> {}

} // namespace geom

// template <typename... Channels> struct ColorG {
//   using tupple_t = std::tuple<Channels...>;
//   using type = ColorG<Channels...>;
//
//   tupple_t channels;
//
//   ColorG(Channels... args) : channels(args...) {}
//
//   template <class ChannelType> constexpr auto &get_channel() noexcept {
//     return std::get<ChannelType>(channels);
//   }
//   template <class ChannelType>
//   constexpr const auto get_channel() const noexcept {
//     return std::get<ChannelType>(channels);
//   }
//
//   static constexpr type blend(const type &lhs, const type &rhs) {
//
//     return type{
//         lhs.get_channel<Channels>() + rhs.get_channel<Channels>()...,
//     };
//   }
// };
//
// namespace detail {
// struct Red {};
// struct Green {};
// struct Blue {};
// struct Hue {};
// struct Saturaton {};
// struct Luminance {};
//
// using RedByte = TypedScalar<std::uint8_t, Red>;
//
// using GreenByte = TypedScalar<std::uint8_t, Green>;
// using BlueByte = TypedScalar<std::uint8_t, Blue>;
// } // namespace detail
//
// struct RGB : ColorG<detail::RedByte, detail::GreenByte, detail::BlueByte> {
//   using ColorG::ColorG;
//
//   constexpr detail::RedByte &red() noexcept {
//     return get_channel<detail::RedByte>();
//   }
//
//   constexpr detail::GreenByte &green() noexcept {
//     return get_channel<detail::GreenByte>();
//   }
//
//   constexpr detail::BlueByte &blue() noexcept {
//     return get_channel<detail::BlueByte>();
//   }
// };
//
// }
// template <typename C1, typename C2, typename C3> struct Color3 {
//
//   C1 c1;
//   C2 c2;
//   C3 c3;
// };
//
// } // namespace geom
