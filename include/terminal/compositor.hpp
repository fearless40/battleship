#pragma once

#include "soa.hpp"
#include "soamemorylayout.hpp"
#include <compare>
#include <iterator>
#include <vector>

namespace term {

class Compositor {
public: // Types
  class Handle {
  private:
    unsigned int external_value{0};
    unsigned int map_index{0};

  public:
    std::strong_ordering operator<=>(const Handle &other) const = default;
    friend Compositor;
  };

  struct Rect {
    int x, y;
    int w, h;
  };

private:
  struct AABB {
    int x, y;
    int x2, y2;

    operator Rect() { return {x, y, x2 - x, y2 - y}; }
  };

  template <typename Tag> struct Range1D {
    int x, x2;
    std::strong_ordering operator<=>(const Range1D<Tag> &other) const = default;
    friend constexpr bool intersects(const Range1D<Tag> &r1,
                                     const Range1D<Tag> &r2) {
      return r1.x >= r2.x && r1.x <= r2.x2 || r1.x2 >= r2.x && r1.x2 <= r2.x2;
    };

    constexpr bool contains(int value) const noexcept {
      return value >= x && value <= x2;
    };

    struct X {};
    struct Y {};

    using XPos = Range1D<X>;
    using YPos = Range1D<Y>;
    using ZOrder = int;

    bool composition_dirty_{false};

    // These vectors are packed (no holes)
    std::vector<Handle> dirty_layers_;
    std::vector<Handle> visible_layers_;

    using SOA = util::soa::SOA<util::soa::memory_layout::DynamicArray, XPos,
                               YPos, ZOrder, Handle>;
    SOA soa;

  public:
    friend class ScanLineIterator;
    class ScanLineIterator {
      using IteratorSOA = typename SOA::Iterator;

      SOA &soa;
      int max_x_;
      int max_y_;
      int x_;
      int y_;
      std::vector<typename SOA::Iterator> current_line;
      IteratorSOA current_box;

      constexpr auto getx(IteratorSOA &it) { return it.template get<XPos>(); };

      // 0 0 1 1 1 1 1 0 0 0 0 2 2 2 2 2 2 2 0 0 0 5 5 5 5 5 0 0 0
      //     1 1 1 1 1         2 2 3 3 3 2 2       5 5 6 6 6 6

      /* Algo To find image span to return to render
       * find_start_position
       * 0. current = passed_in_start
       * 1. for( auto it = current +1; it < end(soa); ++it )
       *        if it.x > current.x and it.z > current.z then return
       *
       *Front:
       Find fist boxc.x > current.zorder
       on next round swap front with back
       scan from start box, if box contains current sav ecbox as current
       next noc. iof box does not cacache then skip until x is greater than
       current.x
       */

      IteratorSOA find_front(IteratorSOA start_at) {
        auto range_start = getx(start_at);

        for (IteratorSOA it = start_at + 1; it != soa.end(); ++it) {
          if (getx(it).x >= range_start.x && getz(it) >= z) {
            return it;

            continue;
          }
        }

      public:
        // Returns the image and number of pixles it represents
        std::pair<Handle, int> operator*() {
          if (x_ > max_x_) {
            // Reset cache and advance a line
            ++y_;
            current_line.clear();

            // Scan for onle the items in the current line
            for (auto it = soa.begin(); it != soa.end(); ++it) {
              if (it.template get<YPos>().contains(y())) {
                current_line.push_back(it);
              };
            }

            // Now sort the line
            std::sort(current_line.begin(), current_line.end(),
                      [](auto &l, auto &r) {
                        return l.template get<XPos>() < r.template get<XPos>();
                      });
          }

          auto what_to_render = current_line.begin();
          // Find the first one that contains x()
          for (auto &it : current_line) {
            if (it->template get<XPos>().contains(x_)) {
              what_to_render = what_to_render.template get<ZOrder>() <
                                       it.template get<ZOrder>()
                                   ? it
                                   : what_to_render;
            }
          }

          int last_x = 0;
          // Find the end of the current image
          for (auto next_item = what_to_render; next_item != current_line.end();
               ++next_item) {

            auto nZ = next_item.template get<ZOrder>();
            auto oZ = what_to_render.template get<ZOrder>();

            if (nZ < oZ) {
              // Then ignore next_item
              continue;
            } else {
              // determine if they overlap
              auto nx_range = next_item.template get<XPos>();
              auto ox_range = what_to_render.template get<XPos>();

              if (ox_range.x2 - nx_range.x1 > 0) {
                // They overlap
                last_x = nx_range.x1;
                break;
              } else {
                last_x = ox_range.x2;
              }
            }
          }

          x_ = last_x + 1;
        };

        // Advances to the next pair of image and width
        ScanLineIterator &operator++();

        // Returns which line number it is on
        int y();

        // Returns what the next x position will be
        int x();

        Handle get_base_layer() const { return {}; };
        Handle new_layer(Rect position, int zOrder);
        void layer_dirty(Handle layer);
        void layer_is_transperant(Handle layer);
        void layer_is_opaque(Handle layer);
        void hide(Handle layer);
        void show(Handle layer);
        void move(Handle layer, const Rect &position);
        void move(Handle layer, int zOrder);
        void move(Handle layer, const Rect &position, int zOrder);

        void flatten_as_scanlines();
        void flatten_as_rects();
      };
    } // namespace term
