#pragma once

#include <compare>
#include <concepts>
namespace geom {

template <typename UnderlyingT, typename TAG, typename UnitT>
struct TypedPosition {
  using underlying_t = UnderlyingT;
  using difference_type = underlying_t;
  using type = TypedPosition<underlying_t, TAG, UnitT>;

  template <typename ValueT>
    requires std::convertible_to<underlying_t, ValueT>
  constexpr TypedPosition(ValueT val) : value(static_cast<underlying_t>(val)) {}

  constexpr TypedPosition() : value(0) {}

  constexpr TypedPosition(const type &other) : value(other.underlying()) {}

  underlying_t value;

  constexpr underlying_t underlying() const noexcept { return value; }
  constexpr std::strong_ordering
  operator<=>(type const &other) const noexcept = default;

  // constexpr bool operator==(type const &other) const noexcept = default;
  // constexpr bool operator!=(type const &other) const noexcept = default;

  constexpr type &operator+=(type const &other) noexcept {
    value += other.value;
    return *this;
  }
  constexpr type &operator-=(type const &other) noexcept {
    value -= other.value;
    return *this;
  }
  constexpr type &operator++() noexcept {
    ++value; // += static_cast<underlying_t>(1);
    return *this;
  }
  constexpr type operator++(int) noexcept {
    type ret = *this;
    ++value; // += static_cast<underlying_t>(1);
    return ret;
  }
  constexpr type &operator--() noexcept {
    --value; // -= static_cast<underlying_t>(1);
    return *this;
  }

  constexpr type &operator--(int) noexcept {
    auto ret = *this;
    --value; // -= static_cast<underlying_t>(1);
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

  constexpr type &operator=(const type &other) noexcept {
    value = other.underlying();
    return *this;
  }

  // friend constexpr type operator+(const type &left,
  //                                 const type &right) noexcept {
  //   return type{left.value + right.value};
  // }
  // friend constexpr type operator-(const type &left,
  //                                 const type &right) noexcept {
  //   return type{left.value - right.value};
  // }
};

} // namespace geom
