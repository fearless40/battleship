#include "rowcol.hpp"
#include "baseconv.hpp"
#include "randomutil.hpp"
#include <charconv>
#include <format>
#include <string_view>

namespace battleship {
std::string RowCol::as_base26_fmt() const {
  std::string result{base26::to_string(col.size)};
  result.append(std::to_string(row.size));
  return result;
}

std::string RowCol::as_colrow_fmt() const {
  return std::format("{},{}", col.size, row.size);
}

RowCol RowCol::random(Row maxRow, Col maxCol) {
  return RowCol{Row{randomns::between<std::uint16_t>(0, maxRow.size)},
                Col{randomns::between<std::uint16_t>(0, maxCol.size)}};
}

bool from_chars(std::string_view const value, std::uint16_t &out) {
  auto err = std::from_chars(value.data(), value.data() + value.size(), out);
  if (err.ec == std::errc())
    return true;
  return false;
}

RowCol parse_comma_fmt(const std::string_view value) {
  auto pos = value.find_first_of(",");
  auto colText = value.substr(0, pos);
  auto rowText = value.substr(pos + 1);

  if (colText.size() == 0 || rowText.size() == 0) {
    return {};
  }

  RowCol ret;

  if (from_chars(colText, ret.col.size) && from_chars(rowText, ret.row.size)) {
    return ret;
  }
  return {};
}

/**
 * @brief Converts a trimmed string (no whitespace allowed base26 or col,row
 * format)
 * @param value all whitespace must be trimmed otherwise evalutes to error
 * @return RowCol object initalized with value
 */
RowCol RowCol::from_string(std::string_view const value) {
  if (value.size() < 2)
    return {};

  if (value[0] <= '9' and value[0] >= '0') {
    // Assume comma format
    return parse_comma_fmt(value);
  }

  auto pos = value.find_first_of("0123456789");
  if (pos == std::string_view::npos) {
    return {};
  }

  auto letters = value.substr(0, pos);

  RowCol ret;
  if (from_chars(value.substr(pos), ret.row.size)) {
    ret.col.size = static_cast<std::uint16_t>(base26::from_string(letters));
    return ret;
  }

  return {};
}
} // namespace battleship
