#pragma once

#include "compositor_shared.hpp"
#include "rect.hpp"
#include "scanline_stack_render.hpp"
#include "scanlinerender_painter.hpp"

namespace term {

class Compositor {
public: // Types
private:
  bool composition_dirty_{false};

  // These vectors are packed (no holes)
  std::vector<compositor::Handle> dirty_layers_;
  std::vector<compositor::Handle> visible_layers_;

  compositor::SOA soa;

private:
  int max_x_{80};
  int max_y_{80};
  compositor::Handle next_handle_{};

  compositor::Handle next_handle() {
    next_handle_.external_value += 1;
    next_handle_.map_index += 1;
    return next_handle_;
  }

  void new_layer_with_handle(util::IntRect position, int zOrder,
                             compositor::Handle handle) {
    soa.push_back({position.x, position.w + position.x},
                  {position.y, position.h + position.y}, zOrder, handle);
  }

public:
  Compositor() { new_layer_with_handle({0, 0, max_x_, max_y_}, 0, {}); }
  Compositor(int max_x, int max_y) : max_x_(max_x), max_y_(max_y) {
    new_layer_with_handle({0, 0, max_x_, max_y_}, 0, {});
  }

  compositor::Handle get_base_layer() const { return {}; };
  compositor::Handle new_layer(util::IntRect position, int zOrder) {
    new_layer_with_handle(position, zOrder, next_handle());
    return next_handle_;
  };

  compositor::stack::Render get_stack_render() {
    return compositor::stack::Render(soa, util::AABB{0, max_x_, 0, max_y_});
  }

  compositor::stack::StackRender get_stack_render_old() {
    return compositor::stack::StackRender(soa, max_x_, max_y_);
  }

  compositor::ScanLinePainter get_painter_render() {
    return compositor::ScanLinePainter(soa, max_x_, max_y_);
  }

  // void layer_dirty(Handle layer);
  // void layer_is_transperant(Handle layer);
  // void layer_is_opaque(Handle layer);
  // void hide(Handle layer);
  // void show(Handle layer);
  // void move(Handle layer, const Rect &position);
  // void move(Handle layer, int zOrder);
  // void move(Handle layer, const Rect &position, int zOrder);

  // void flatten_as_scanlines();
  // void flatten_as_rects();
};
}; // namespace term
