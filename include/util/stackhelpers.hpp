#pragma once
#include <concepts>

namespace util::stack {

template <class StackT>
concept stack_like = requires(StackT t) {
  t.pop();
  t.top();
  { t.size() } -> std::convertible_to<std::size_t>;
};

template <stack_like StackT> constexpr void clear(StackT &stack) {
  while (stack.size() > 0)
    stack.pop();
};

namespace debug {
template <stack_like Stack, typename Presenter>
void print_stack(Stack stack, Presenter &&p) {
  while (stack.size() > 0) {
    p(stack.top());
    stack.pop();
  }
}
} // namespace debug

} // namespace util::stack
