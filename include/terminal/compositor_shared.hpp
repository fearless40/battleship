#pragma once
#include "soa.hpp"
#include "soamemorylayout.hpp"
#include "typed_position.hpp"
#include <compare>
#include <onedrange.hpp>
#include <rect.hpp>
#include <xy.hpp>

namespace term::compositor {
class Handle {
public:
  unsigned int external_value{0};
  unsigned int map_index{0};

public:
  unsigned int index() { return external_value; }
  std::strong_ordering operator<=>(const Handle &other) const = default;
};

namespace detail {
struct CharUnit {};

} // namespace detail

using X = geom::TypedPosition<int, geom::detail::X_TAG, detail::CharUnit>;
using Y = geom::TypedPosition<int, geom::detail::Y_TAG, detail::CharUnit>;
using Width = geom::TypedDimension<X>;
using Height = geom::TypedDimension<Y>;

using XPos = geom::Range1D<Width>;
using YPos = geom::Range1D<Height>;
using ZOrder = int;

using Rect = geom::TypedRect<Width, Height>;

using SOA = util::soa::SOA<util::soa::memory_layout::DynamicArray, XPos, YPos,
                           ZOrder, Handle>;

template <class HANDLE> struct ContigousRenderRegion {
  HANDLE handle;
  XPos range;

  //   constexpr auto begin() { return xStart; }
  //   constexpr auto end() { return xEnd; }
};

using CRR = ContigousRenderRegion<Handle>;

namespace soa {

using IteratorSOA = typename SOA::Iterator;
static constexpr auto getx(IteratorSOA &it) { return it.template get<XPos>(); };
static constexpr auto gety(IteratorSOA &it) { return it.template get<YPos>(); };
static constexpr auto x(IteratorSOA &it) { return it.template get<XPos>().p; };
static constexpr auto x2(IteratorSOA &it) {
  return it.template get<XPos>().p2;
};
static constexpr auto zorder(IteratorSOA &it) {
  return it.template get<ZOrder>();
};
static constexpr auto handle(IteratorSOA &it) {
  return it.template get<Handle>();
}
} // namespace soa

} // namespace term::compositor
