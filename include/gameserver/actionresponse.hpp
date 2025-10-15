#pragma once

#include "gamelayout.hpp"

namespace Response {
enum class ResponseValue { hit, miss, sink };

struct Response {
  ResponseValue value;
  ShipDefinition ship;
};

constexpr Response Miss() { return {ResponseValue::miss, {}}; };

constexpr Response Hit() { return {ResponseValue::hit, {}}; };

constexpr Response Sink(ShipDefinition ship) {
  return {ResponseValue::sink, ship};
}
} // namespace Response
