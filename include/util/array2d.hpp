#pragma once

#include <algorithm>
#include <optional>
#include <rowcol.hpp>
#include <vector>

/**
 * @description A 2D array of values defined by [0,nbrRows), [0,nbrCols)
 * @note Not inclusive of the Row and Col
 */

template <typename T> struct Array2D {
  Row m_rows;
  Col m_cols;
  std::vector<T> m_values;

  Array2D() : m_rows(0), m_cols(0) {};
  explicit Array2D(Row nbrRows, Col nbrCols)
      : m_rows(nbrRows), m_cols(nbrCols) {
    m_values.reserve(m_rows.size * m_cols.size);
  }

  void resize(const Row &rowSize, const Col &colSize) {
    if (rowSize != m_rows || colSize != m_cols) {

      m_values.resize(rowSize.size * colSize.size);
      m_rows = rowSize;
      m_cols = colSize;
    }
  }

  void fill(const T &value) { std::ranges::fill(m_values, value); }

  std::optional<RowCol> find_index_from(const T &value,
                                        RowCol starting_pos = RowCol(Row(0),
                                                                     Col(0))) {
    if (!is_valid_index(starting_pos))
      return {};

    std::size_t start_index = to_index(starting_pos);
    std::size_t index = start_index;

    do {

      if (m_values[index] == value)
        return from_index(index);

      ++index;
      if (index >= m_values.size())
        index = 0;
    } while (index != start_index);
    return {};
  }

  constexpr T &operator()(Row r, Col c) { return m_values[to_index({r, c})]; }

  constexpr bool is_valid_index(const RowCol &pos) const {
    return (pos.row.size >= 0 && pos.row.size < m_rows.size) &&
           (pos.col.size >= 0 && pos.col.size < m_cols.size);
  }

  constexpr T &operator[](const RowCol &pos) { return m_values[to_index(pos)]; }
  constexpr const T &operator[](const RowCol &pos) const {
    return m_values[to_index(pos)];
  }

  constexpr void set(const RowCol &pos, T const &value) {
    if (is_valid_index(pos)) {
      m_values[to_index(pos)] = value;
    }
  }

  constexpr std::optional<T &> at(const RowCol &pos) {
    if (!is_valid_index(pos))
      return {};

    auto index = to_index(pos);
    return m_values[index];
  }

  constexpr std::size_t to_index(const RowCol &pos) const {
    return pos.row.size * m_cols.size + pos.col.size;
  }

  RowCol from_index(std::size_t index) {
    auto val = std::div(index, m_cols.size);
    return RowCol{Row{static_cast<unsigned short>(val.quot)},
                  Col{static_cast<unsigned short>(val.rem)}};
  }
};
