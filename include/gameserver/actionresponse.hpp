#pragma once

#include "gamelayout.hpp"

struct Response {
  enum class ResponseValue { hit, miss, sink };
  ResponseValue value;
  ShipDefinition ship;
  static constexpr Response Miss() { return {ResponseValue::miss, {}}; };

  static constexpr Response Hit() { return {ResponseValue::hit, {}}; };

  static constexpr Response Sink(ShipDefinition ship) {
    return {ResponseValue::sink, ship};
  }

  constexpr bool is_hit() const noexcept {
    return value == ResponseValue::hit;
  };
  constexpr bool is_miss() const noexcept {
    return value == ResponseValue::miss;
  };
  constexpr bool is_sunk() const noexcept {
    return value == ResponseValue::sink;
  };
};

// namespace Response
