/**
* Copyright (c) 2017-present, Facebook, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-style license found in the
* LICENSE file in the root directory of this source tree. An additional grant
* of patent rights can be found in the PATENTS file in the same directory.
*/

#include "comm_ai.h"
#include "save2json.h"
#include "../xai/decisions.h"
#include "../xai/decision.h"
#include <vector>

///////////////////////////// Web TCP AI ////////////////////////////////
bool TCPAI::Act(const GameEnv &env, bool /* must_act */) {
  // First we send the visualization.
  if (_receiver->GetTick() >= _vis_after) send_vis(save_vis_data(env));

  std::string msg;
  while (queue_.try_dequeue(msg)) {
     _raw_converter.Process(env, msg, _receiver);
  }
  return true;
}

string TCPAI::save_vis_data(const GameEnv& env) const {
  bool is_spectator = (_player_id == INVALID);

  json game;
  env.FillHeader<save2json, json>(*_receiver, &game);
  env.FillIn<save2json, json>(_player_id, *_receiver, &game);

  if (is_spectator) {
    save2json::Save(*this, &game);
    // Get all current issued commands.
    save2json::SaveCmd(*_receiver, INVALID, &game);

    // Save the current replay progress.
    int replay_size = _receiver->GetLoadedReplaySize();
    if (replay_size > 0) {
      game["progress_precent"] = _receiver->GetTick() * 100 / _receiver->GetLoadedReplayLastTick();
    }
  } else {
    save2json::Save(*this, &game);
    save2json::SaveCmd(*_receiver, _player_id, &game);
  }
  json message;
  std::string game_state_message_type("gameState");
  save2json::BuildTypedMessage(&game, game_state_message_type, &message);
  return message.dump();
}
bool TCPAI::send_vis(const string &s) {
  server_->send(s);
  return true;
}
bool TCPAI::send_decision_info(){
  std::string dir("/home/irvineje/exact/ELF-OSU/data/decisions");
  xai::Decisions decisions(dir);
  std::vector<xai::Decision> decs = decisions.get_decisions();
  std::vector<xai::Decision>::const_iterator iter = decs.begin();
  std::vector<json> json_decs;
  while (iter != decs.end()){
      xai::Decision dec = *iter;
      std::string action = dec.getAction();
      int frame = dec.getFrame();
      json decision_json = { {"frame", frame}, {"action", action} };
      json_decs.push_back(decision_json);
      iter++;
  }
  json message;
  std::string decision_info_message_type("decisionInfo");
  //std::string action("attack");
  //int frame = 100;
  //json info1 = { {"frame", frame}, {"action", action} };
  //json info2 = { {"frame", 200}, {"action", "retreat"} };
  //json info3 = { {"frame", 300}, {"action", "build"} };
  json info;
  //info["decisions"] = { info1, info2, info3 };
  info["decisions"] = json_decs;
  save2json::BuildTypedMessage(&info, decision_info_message_type, &message);
  const string s(message.dump());
  server_->send(s);
  return true;
}
