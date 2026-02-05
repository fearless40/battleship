#pragma once

#include <compare>
#include <concepts>
namespace geom {

template <typename UnderlyingT, typename TAG, typename UnitT>
struct TypedPosition {
  using underlying_t = UnderlyingT;
  using type = TypedPosition<underlying_t, TAG, UnitT>;

  template <typename ValueT>
    requires std::convertible_to<underlying_t, ValueT>
  TypedPosition(ValueT val) : value(static_cast<underlying_t>(val)) {}

  TypedPosition() : value(0) {}

  underlying_t value;

  constexpr underlying_t underlying() const noexcept { return value; }
  std::strong_ordering operator<=>(type const &other) const noexcept = default;

  constexpr bool operator==(type const &other) const noexcept = default;
  constexpr bool operator!=(type const &other) const noexcept = default;

  constexpr type &operator+=(type const &other) const noexcept {
    value += other.value;
    return *this;
  }
  constexpr type &operator-=(type const &other) const noexcept {
    value -= other.value;
    return *this;
  }
  constexpr type &operator++() const noexcept {
    value += static_cast<underlying_t>(1);
    return *this;
  }
  constexpr type operator++(int) const noexcept {
    auto ret = value;
    value += static_cast<underlying_t>(1);
    return ret;
  }
  constexpr type &operator--() const noexcept {
    value -= static_cast<underlying_t>(1);
    return *this;
  }

  constexpr type &operator--(int) const noexcept {
    auto ret = *this;
    value -= static_cast<underlying_t>(1);
    return ret;
  }

  constexpr type &operator*=(underlying_t value_) noexcept {
    value *= value_;
    return *this;
  }

  constexpr type &operator=(underlying_t value_) noexcept {
    value = value_;
    return *this;
  }

  friend constexpr type operator+(const type &left,
                                  const type &right) noexcept {
    return type{left.value + right.value};
  }
  friend constexpr type operator-(const type &left,
                                  const type &right) noexcept {
    return type{left.value - right.value};
  }
};

} // namespace geom
