#pragma once

#include <compare>
namespace geom {

namespace detail {
struct DefaultConverter {};
} // namespace detail

template <typename T, typename Tag,
          typename Converter = detail::DefaultConverter>
struct TypedScalar {

  using type = TypedScalar<T, Tag, Converter>;
  using underlying_t = T;

  underlying_t value;

  constexpr std::strong_ordering
  operator<=>(type const &other) const noexcept = default;

  constexpr underlying_t underlying() const noexcept { return value; }

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
