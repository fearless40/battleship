#pragma once

#include "actionresponse.hpp"
#include "gamelayout.hpp"
#include "rowcol.hpp"
#include "ship.hpp"
#include <array>
#include <bitset>
#include <optional>

inline namespace v1 {

class ClassicPlayer {
public:
private:
public:
  // Returns true if shot is allowed
  bool fire_single_shot(RowCol value);

  Response last_guess_result() const noexcept;
};

class ClassicGameServer {
  using history = std::pair<RowCol, Response>;
  struct ShipHit {
    std::bitset<32> hits;
    ShipDefinition id;
    constexpr bool is_sunk() const noexcept { return hits.count() == id.size; }
  };

  struct Player {
    int user_id{0};
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

public:
  // Methods
  ClassicGameServer(const GameLayout &layout) : mLayout(layout) {}

  std::optional<ClassicPlayer> set_player(int user_id, Ship::Ships &&ships) {
    if (mGame_started)
      return {};

    auto &player = mPlayers[mCurrent_player];
    player.ships = std::move(ships);
    player.user_id = user_id;
    player.hits.reserve(player.ships.size());

    for (const auto &s : player.ships) {
      player.hits.emplace_back(std::bitset<32>{}, s.id());
    }

    void set_first_player(const ClassicPlayer &player);

    bool game_finished() const noexcept;

    std::size_t current_round();
    bool next_round(); // Returns true if game is not won;
  };
} // namespace v1
