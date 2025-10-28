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
};

// namespace Response
