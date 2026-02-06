#include "relative_dimension.hpp"
#include "typed_position.hpp"

namespace geom {

namespace detail {
struct AbsoluteUNIT {};
struct X_TAG {};
struct Y_TAG {};

} // namespace detail

using X = TypedPosition<int, detail::X_TAG, detail::AbsoluteUNIT>;
using Y = TypedPosition<int, detail::Y_TAG, detail::AbsoluteUNIT>;
using Width = TypedDimension<X>;
using Height = TypedDimension<Y>;

} // namespace geom
