#pragma once

#include "rowcol.hpp"
#include "ship.hpp"
class Rules {

}

struct ClientState {
  battleship::Array2d<KnownMap> map;
  std::vector<ActionDefinitions> allowed_actions;
  std::vector<Tokens> tokens;
  std::vector<Effects> effects;
  std::vector<battleship::Ships> ships;
  std::vector<Action> past_actions;
};

class Action {
  virtual void run(ClientState *ownerState, ClientState *targetState) = 0;
};

class FireShot : public Action {
  ActionResponse run(ClientState *ownerState,
                     ClientState *targetState) override {
    auto hit = targetState->ships.hit(postion_to_shoot);
    if (hit) {
      if (sunk_ship)
        Respone::Sink(ship_id);
      else
        Response::Hit(ship_id);
    }
    return Response::Miss();
  }

private:
  battleship::RowCol postion_to_shoot;
}

class Server {
public:
  void new_game(battleship::GameLayout layout, std::size_t nbrClients);
  ClientState get_client_state(CLientID);
  ActionResponse play_action(Action *action, ClientID fromID, CLientID target);
}
