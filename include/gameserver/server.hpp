#pragma once

inline namespace v1 {

class GameServer {

public:
  void new_game(std::size_t number_players);

  const Player &get_player(std::size_t index);
  bool submit_player(const Player &p);

  ActionResponse submit_action(Action &action);
  ActionResponse validate_action(Action &action);

  std::size_t current_round();
  std::size_t advance_round();
}
} // namespace v1
