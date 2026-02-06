#pragma once
#include <compare>

namespace geom {

template <typename TypedPositionT> struct TypedDimension {
  using position_t = TypedPositionT;
  using underlying_t = typename position_t::underlying_t;
  using type = TypedDimension<TypedPositionT>;

  underlying_t value;

  constexpr underlying_t underlying() const noexcept { return value; }

  constexpr TypedDimension(position_t const &first, position_t const &second)
      : value(second.underlying() - first.underlying()) {}

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

  constexpr type &operator*=(underlying_t value_) const noexcept {
    value *= value_;
    return *this;
  }
  constexpr type &operator/=(underlying_t value_) const noexcept {
    value /= value_;
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
