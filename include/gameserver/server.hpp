#pragma once

#include "actionresponse.hpp"
#include "gamelayout.hpp"
#include "rowcol.hpp"
#include "ship.hpp"
#include <algorithm>
#include <array>
#include <bitset>
#include <compare>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <optional>
#include <span>

inline namespace v1 {

namespace detail {
template <class Tag> struct TypedID {
  int value{0};
  std::strong_ordering operator<=>(const TypedID<Tag> &other) const = default;
};
struct GameID {};
struct UserID {};

} // namespace detail
using ID = detail::TypedID<detail::GameID>;
using UserID = detail::TypedID<detail::UserID>;

class ClassicGameServer {
public:
  using history = std::pair<RowCol, Response>;
  class ClientPlayer {
    ClassicGameServer &mServer;
    ID mId;
    UserID mUser_id;

  public:
    struct ShipStatus {
      ShipDefinition id{0};
      std::size_t number_hits{0};
      bool sunk{false};
    };

  public:
    ClientPlayer(ClassicGameServer &serv, ID playerid, UserID user_id)
        : mServer(serv), mId(playerid), mUser_id(user_id) {};

    bool fire_single_shot(RowCol guess) { return false; }

    Response last_response() const noexcept {
      const auto player = mServer.get_player(mId);
      return player.shots.back().second;
    }

    const std::span<const history> prior_shots() const noexcept {
      const auto &player = mServer.get_player(mId);
      return {player.shots};
    }

    const int ship_hits(ShipDefinition ship) const noexcept {
      const auto &player = mServer.get_player(mId);
      auto it = std::ranges::find(player.hits, ship, &ShipHit::id);
      if (it == player.hits.end())
        return 0;

      return it->hits.count();
    }

    const std::vector<ShipStatus> ships_all_hits() const {

      std::vector<ShipStatus> results;
      results.reserve(mServer.number_ships());

      auto const &p = mServer.get_player(mId);
      std::transform(p.hits.begin(), p.hits.end(), std::back_inserter(results),
                     [](auto const &hit) -> ShipStatus {
                       return {hit.id, hit.hits.count(),
                               hit.hits.count() == hit.id.size};
                     });

      return results;
    };

    constexpr auto id() const noexcept { return mId; }

    constexpr auto user_id() const noexcept { return mUser_id; }
  };

  friend class ClientPlayer;

private:
  struct ShipHit {
    std::bitset<32> hits;
    ShipDefinition id;
    constexpr bool is_sunk() const noexcept { return hits.count() == id.size; }
  };

  struct Player {
    ID id;
    UserID user_id;
    std::vector<ShipHit> hits;
    Ship::Ships ships;
    std::vector<history> shots;
  };

private:
  // Data Members
  GameLayout mLayout{};
  bool mGame_started{false};
  bool mGame_won{false};
  std::size_t mRound_count{0};
  std::array<Player, 2> mPlayers;
  std::size_t mCurrent_player{0};

private:
  // Private methods
  constexpr std::size_t number_ships() const noexcept {
    return mLayout.maxShipSize.size - mLayout.minShipSize.size;
  }

  bool did_player_lose(ID player_id) const noexcept {
    auto const &p = mPlayers[player_id.value];
    auto count_sunk =
        std::count_if(p.hits.begin(), p.hits.end(),
                      [](const ShipHit &hit) { return hit.is_sunk(); });
    return count_sunk == number_ships();
  }

  bool is_game_over() const noexcept {
    for (auto &p : mPlayers) {
      if (did_player_lose(p.id) == true)
        return true;
      ;
    }
    return false;
  }

  const Player &get_player(ID id) const { return mPlayers[id.value]; }

  Player &get_target_of_player(ID id) noexcept {
    if (id == ID(0))
      return mPlayers[1];
    if (id == ID(1))
      return mPlayers[0];
    return mPlayers[0];
  }

  void fire_single_shot(const ClientPlayer &player, RowCol guess) {
    if (player.id() != ID(mCurrent_player))
      return;

    auto enemy = get_target_of_player(player.id());
    auto current = mPlayers[player.id().value];

    if (auto ship_at = Ship::ship_at_position(enemy.ships, guess); ship_at) {
      auto ship = ship_at.value();
      if (auto section_opt = ship.ship_section_hit(guess); section_opt) {
        auto index = mLayout.shipdef_to_index(ship.id());
        enemy.hits[index].hits.set(section_opt.value(), true);
      }

    public:
      // Methods
      ClassicGameServer(const GameLayout &layout) : mLayout(layout) {}

      std::optional<ClientPlayer> set_player(UserID user_id,
                                             Ship::Ships && ships) {
        if (mGame_started)
          return {};

        auto &player = mPlayers[mCurrent_player];
        player.id = ID(mCurrent_player);
        player.ships = std::move(ships);
        player.user_id = user_id;
        player.hits.reserve(player.ships.size());

        for (const auto &s : player.ships) {
          player.hits.emplace_back(std::bitset<32>{}, s.id());
        }

        ++mCurrent_player;
        return ClientPlayer(*this, player.id, player.user_id);
      }

      void set_first_player(const ClientPlayer &player) {
        if (mGame_started)
          return;

        mCurrent_player = player.id().value;
      }

      bool game_finished() const noexcept { return mGame_won; }

      std::size_t current_round() { return mRound_count; }
      bool next_round() {
        if (!mGame_started) {
          mGame_started = true;
          mGame_won = false;
          mRound_count = 0;
          return true;
        }

        ++mRound_count;
        ++mCurrent_player;
        if (mCurrent_player > 1)
          mCurrent_player = 0;

        if (is_game_over()) {
          mGame_won = true;
          mGame_started = false;
          return false;
        }

        return true;
      }

      // Returns true if game is not won;
    };
  } // namespace v1
