#pragma once

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <type_traits>
namespace util::soa::memory_layout {
struct DynamicArray {

  static const bool is_fixed_capacity = false;
  static const bool is_contigious = true;
  static const std::size_t capacity = 0;

  using index_t = std::size_t;

  // Array types
  template <typename T> using value_t = std::add_pointer_t<T>;

  template <typename data_t>
  using reference_t = std::add_pointer_t<
      value_t<data_t>>; // std::add_lvalue_reference_t<data_t>;

  template <typename data_t> using pointer_t = value_t<data_t>;

  // Single Item in the array types

  template <typename data_t> using item_t = data_t;

  template <typename data_t>
  using item_reference_t = std::add_lvalue_reference_t<data_t>;

  template <typename data_t>
  using item_pointer_t = std::add_pointer_t<std::decay<data_t>>;

  template <typename data_t>
  using arg_type_t = std::conditional_t<sizeof(data_t) < sizeof(void *), data_t,

                                        item_reference_t<data_t>>;

  // Functions for working with a single array

  template <typename data_t>
  static void allocate(reference_t<data_t> old, std::size_t oldsz,
                       std::size_t newsz) {
    if constexpr (std::is_trivially_copyable_v<data_t>) {
      value_t<data_t> new_array = new data_t[newsz];
      std::memcpy(*old, new_array, oldsz);
      *old = new_array;
    } else {

      value_t<data_t> new_array = new data_t[newsz];
      for (std::size_t index = 0; index < oldsz; ++index)
        new_array[index] = std::move(old[index]);
      *old = new_array;
    }
  }
  template <typename data_t>
  static constexpr auto end(const data_t *value, std::size_t endIndex) {
    return value + endIndex;
  }

  template <typename data_t>
  // requires ( data_t.begin() )
  static constexpr auto begin(const data_t *value) {
    return value;
  }

  template <typename data_t>
  // requires ( data_t.operator [] )
  static constexpr item_reference_t<data_t> at(data_t *value, index_t index) {
    return value[index];
  }

  template <typename data_t>
  // requires ( data_t.operator [] )
  static constexpr item_pointer_t<data_t> pointer_at(data_t *value,
                                                     index_t index) {
    return &value[index];
  }

  template <typename data_t>
  // requires ( data_t.operator [] )
  static constexpr void set(reference_t<data_t> array, index_t index,
                            const data_t &item_value) {
    array[index] = item_value;
  }

  template <typename data_t>
  // requires (data_t.data() )
  static constexpr pointer_t<data_t> get_pointer(data_t *value) {
    return value;
  }
};

template <unsigned int MAX_CAPACITY> struct FixedArray {

  static const bool is_fixed_capacity = true;
  static const bool is_contigious = true;
  static const std::size_t capacity = MAX_CAPACITY;

  using index_t = std::size_t;

  // Array types
  template <typename data_t> using value_t = std::array<data_t, MAX_CAPACITY>;

  template <typename data_t>
  using reference_t = std::add_lvalue_reference_t<value_t<data_t>>;

  template <typename data_t>
  using pointer_t = std::add_pointer_t<value_t<data_t>>;

  // Single Item in the array types

  template <typename data_t>
  using item_t = typename value_t<data_t>::value_type;

  template <typename data_t>
  using item_reference_t = typename value_t<data_t>::reference;

  template <typename data_t>
  using item_pointer_t = typename value_t<data_t>::pointer;

  template <typename data_t>
  static value_t<data_t> allocate(value_t<data_t>, std::size_t oldsz,
                                  std::size_t newsz) {
    return {};
  }

  template <typename data_t>
  // requires (data_t) { data_t.end() }
  static constexpr auto end(const data_t &value, std::size_t endIndex) {
    return value.begin() + endIndex;
  }

  template <typename data_t>
  // requires ( data_t.begin() )
  static constexpr auto begin(const data_t &value) {
    return value.begin();
  }

  template <typename data_t>
  // requires ( data_t.operator [] )
  static constexpr typename data_t::reference at(data_t &value, index_t index) {
    return value[index];
  }

  template <typename data_t>
  // requires ( data_t.operator [] )
  static constexpr typename data_t::pointer pointer_at(data_t &value,
                                                       index_t index) {
    return &value[index];
  }

  template <typename data_t>
  // requires ( data_t.operator [] )
  static constexpr void set(reference_t<data_t> array, index_t index,
                            const data_t &item_value) {
    array[index] = item_value;
  }

  template <typename data_t>
  // requires (data_t.data() )
  static constexpr typename data_t::pointer get_pointer(data_t &value) {
    return value.data();
  }
};
} // namespace util::soa::memory_layout
