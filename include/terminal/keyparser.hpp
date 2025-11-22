#pragma once

#include <cstdint>
#include <string_view>
namespace term::kittykeyprotocol {

enum class KeyPosition : uint8_t { pressed, released };

struct KeyStatus {
  KeyPosition position;
  uint8_t shift : 1;
  uint8_t ctl : 1;
  unsigned char alt : 1;
  unsigned char super : 1;
  unsigned int key;

  constexpr operator bool() { return key != 0; }
};

KeyStatus parse(const std::string_view buffer) noexcept;

} // namespace term::kittykeyprotocol
