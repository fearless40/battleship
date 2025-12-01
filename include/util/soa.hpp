#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstring>
#include <span>
#include <stdexcept>
#include <tuple>
#include <type_traits>

namespace util::soa {

namespace detail {
struct DynamicCapacity {
  std::size_t mCapacity;
  constexpr auto capacity() { return mCapacity; }

  constexpr void set_capacity(std::size_t amount) { mCapacity = amount; }

  constexpr void remove_capacity(std::size_t amount) { mCapacity -= amount; }
};

template <std::size_t amount> struct FixedCapacity {
  constexpr auto capacity() { return amount; }

  constexpr void set_capacity(std::size_t amount_value) { return; }

  constexpr void remove_capacity(std::size_t amount_value) { return; }
};

template <typename policy_t>
using Capacity = std::conditional_t<policy_t::is_fixed_capacity,
                                    detail::FixedCapacity<policy_t::capacity>,
                                    detail::DynamicCapacity>;
} // namespace detail

template <typename protocol_t, typename... structures_t>
class SOA : public detail::Capacity<protocol_t> {
public:
  using storage_t =
      std::tuple<typename protocol_t::template value_t<structures_t>...>;
  using value_t =
      std::tuple<typename protocol_t::template item_t<structures_t>...>;
  using reference_t = std::tuple<
      typename protocol_t::template item_reference_t<structures_t>...>;
  using pointer_t =
      std::tuple<typename protocol_t::template item_pointer_t<structures_t>...>;

  template <typename T>
  using array_storage_t = typename protocol_t::template value_t<T>;

  using index_t = std::size_t;

  using detail::Capacity<protocol_t>::capacity;
  using detail::Capacity<protocol_t>::set_capacity;

private:
  index_t mSize{0};
  storage_t mArrays{};

  template <typename structure_t>
  constexpr array_storage_t<structure_t> &get_array() {
    return std::get<array_storage_t<structure_t>>(mArrays);
  }

public:
  class Iterator {
  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = reference_t;
    using difference_type = std::ptrdiff_t;
    using pointer = void;
    using reference = std::add_lvalue_reference_t<reference_t>;
    using iterator_concept = std::contiguous_iterator_tag;

  private:
    std::size_t mIndex{0};
    SOA *mSOA{nullptr};

  public:
    Iterator() {}
    Iterator(std::size_t index, SOA *soa) : mIndex(index), mSOA(soa) {}
    reference_t operator*() const { return mSOA->at(mIndex); }
    auto &operator++() {
      ++mIndex;
      return *this;
    }
    auto operator++(int) {
      auto tmp = *this;
      ++(*this);
      return tmp;
    }
    auto &operator--() {
      --mIndex;
      return *this;
    }
    auto operator--(int) {
      auto tmp = *this;
      --(*this);
      return tmp;
    }
    auto &operator+=(difference_type amount) {
      mIndex += amount;
      return *this;
    }
    auto &operator-=(difference_type amount) {
      mIndex -= amount;
      return *this;
    }

    friend Iterator operator+(const Iterator &left, difference_type right) {
      Iterator tmp(left);
      return tmp += right;
    }
    friend Iterator operator+(difference_type right, const Iterator &left) {
      Iterator tmp(left);
      return tmp += right;
    }

    friend Iterator operator+(const Iterator &left, const Iterator &right) {
      Iterator tmp(left);
      return tmp += right;
    }
    friend difference_type operator-(const Iterator &left,
                                     const Iterator &right) {
      return left.mIndex - right.mIndex;
    }
    friend Iterator operator-(const Iterator &left, difference_type right) {
      Iterator tmp(left);
      return tmp -= right;
    }

    reference_t operator[](const difference_type amount) const {
      return mSOA->at(amount);
    }

    constexpr std::strong_ordering operator<=>(const Iterator &other) const {
      return mIndex <=> other.mIndex;
    }
    bool operator==(const Iterator &other) const = default;

    std::size_t index() { return mIndex; }

    template <typename T> T &get() { return mSOA->view_at<T>(mIndex); }

    // static_assert(std::destructible<Iterator>);
    // static_assert(std::random_access_iterator<Iterator>);
  };

  constexpr std::size_t size() { return mSize; }

  void push_back(const structures_t &...items) {
    if (capacity() < size() + 1) {
      // Then Allocate

      if constexpr (!protocol_t::is_fixed_capacity) {
        (protocol_t::allocate(get_array<structures_t>(), capacity(),
                              capacity() * 3),
         ...);
        this->set_capacity(capacity() * 3);
      } else {
        throw new std::invalid_argument("Error not a allocating type in SOA.");
      }
    }

    (protocol_t::set(get_array<structures_t>(), mSize, items), ...);
    ++mSize;
  }

  // Todo: make it call destruction operator if one exists
  void remove(Iterator it) {
    if (end() == begin())
      return;

    auto last = end() - 1;

    // This will not handle destruction of items
    if (last == begin()) {
      mSize = 0;
    }

    if (it == last) {
      --mSize;
      return;
    }

    *it = *last;
    --mSize;
  }

  reference_t at(index_t index) {
    return reference_t{protocol_t::at(get_array<structures_t>(), index)...};
  }

  pointer_t pointer_at(index_t index) {
    return pointer_t{
        protocol_t::pointer_at(get_array<structures_t>(), index)...};
  }

  template <typename T> T &view_at(index_t index) {
    return protocol_t::at(get_array<T>(), index);
  }

  template <typename row_t>
  typename protocol_t::template item_pointer_t<row_t> row() {
    return protocol_t::get_pointer(get_array<row_t>());
  }

  template <typename row_t> std::span<row_t> row_span() {
    return std::span<row_t>(protocol_t::get_pointer(get_array<row_t>()), mSize);
  }

  template <typename row_t> auto row_begin() {
    return protocol_t::begin(get_array<row_t>());
  }

  template <typename row_t> auto row_end() {
    return protocol_t::end(get_array<row_t>(), mSize);
  }

  constexpr Iterator begin() { return Iterator(0, this); }

  constexpr Iterator end() { return Iterator(mSize, this); }

  template <typename T> constexpr Iterator find(const T &to_find) {
    const auto to_search = row_span<T>();
    auto foundIt = std::find(to_search.cbegin(), to_search.cend(), to_find);
    if (foundIt != to_search.cend()) {
      return Iterator(std::distance(to_search.cbegin(), foundIt), this);
    }
    return end();
  }
};
} // namespace util::soa
