#pragma once
#include "soa.hpp"
#include "soamemorylayout.hpp"
#include <compare>
#include <onedrange.hpp>

namespace term::compositor {
class Handle {
public:
  unsigned int external_value{0};
  unsigned int map_index{0};

public:
  unsigned int index() { return external_value; }
  std::strong_ordering operator<=>(const Handle &other) const = default;
};

struct X {};
struct Y {};

using XPos = util::Range1D<int, X>;
using YPos = util::Range1D<int, Y>;
using ZOrder = int;

using SOA = util::soa::SOA<util::soa::memory_layout::DynamicArray, XPos, YPos,
                           ZOrder, Handle>;

namespace soa {

using IteratorSOA = typename SOA::Iterator;
static constexpr auto getx(IteratorSOA &it) { return it.template get<XPos>(); };
static constexpr auto gety(IteratorSOA &it) { return it.template get<YPos>(); };
static constexpr auto x(IteratorSOA &it) { return it.template get<XPos>().x; };
static constexpr auto x2(IteratorSOA &it) {
  return it.template get<XPos>().x2;
};
static constexpr auto zorder(IteratorSOA &it) {
  return it.template get<ZOrder>();
};
static constexpr auto handle(IteratorSOA &it) {
  return it.template get<Handle>();
}
} // namespace soa

} // namespace term::compositor
