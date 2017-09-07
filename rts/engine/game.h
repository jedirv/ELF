/**
* Copyright (c) 2017-present, Facebook, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-style license found in the
* LICENSE file in the root directory of this source tree. An additional grant
* of patent rights can be found in the PATENTS file in the same directory.
*/

#ifndef _GAME_H_
#define _GAME_H_

#include <algorithm>
#include <string>
#include <iostream>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#define sleep(n)    Sleep(n)
#endif

#include <queue>
#include <set>
#include "game_env.h"
#include "ai.h"
#include "file_util.h"

struct RTSGameOptions {
    // A map file that specifies the map, the terrain
    string map_filename;

    // A replay file to load. It will contains the locations of bases and units, etc.
    // And action sequence.
    vector<string> load_replay_filenames;

    string save_replay_prefix;
    string snapshot_prefix;
    string snapshot_load;
    string snapshot_load_prefix;

    // State string to load from
    string state_string;

    // Snapshots to load from.
    vector<Tick> snapshots;

    // Initial seed. If seed = 0, then we use time(NULL)
    // When seed != 0, the game should be deterministic (If not then there is a bug somewhere).
    int seed = 0;

    // CmdDumperPrefix
    string cmd_dumper_prefix;

    // By-pass bot actions. Only use cmd stored by replay.
    bool bypass_bot_actions = false;

    // Ticks to be peeked. We only use this when bypass_bot_actions = true.
    set<Tick> peek_ticks;

    // time allowed to spend in main_loop, in milliseconds.
    int main_loop_quota = 0;

    // Whether we show command during simulation.
    int cmd_verbose = 0;

    // Which file the intermediate prompt will be sent to.
    // if output_file == "cout", then it will send to stdout.
    // otherwise open a file and send the prompts to the file.
    string output_file;

    // If output_file is empty, then we check output_stream, if it is not nullptr,
    // we will use it for output.
    ostream *output_stream = nullptr;

    // Max tick for the game to run.
    int max_tick = 30000;

    // Print Interval, in ticks.
    int tick_prompt_n_step = 2000;

    // Whether we save the snapshot using binary format (faster).
    bool save_with_binary_format = true;

    // Handicap_level used in Capture the Flag.
    int handicap_level = 0;

    // the root directory under which snapshot,replay, decision files will be put. If not set, it will just be in the current directory
    string datafiles_root;
    string decision_dir;
    string snapshot_dir;
    string replay_dir;

    string PrintInfo() const {
        std::stringstream ss;

        ss << "Map_filename: " << map_filename << endl;
        ss << "Load replay filenames: " << endl;
        for (const auto &f : load_replay_filenames) ss << "  \"" << f << "\"" << endl;
        ss << "Save replay prefix: \"" << save_replay_prefix << "\"" << endl;
        ss << "Snapshot prefix: \"" << snapshot_prefix << "\"" << endl;
        ss << "Snapshot load: \"" << snapshot_load << "\"" << endl;
        ss << "Snapshot load prefix: \"" << snapshot_load_prefix << "\"" << endl;
        ss << "Snapshots[" << snapshots.size() << "]: ";
        for (const Tick &t : snapshots) ss << t << ", ";
        ss << endl;
        ss << "State string: #size = " << state_string.size() << endl;
        ss << "Seed: " << seed << endl;
        ss << "Cmd Dumper prefix: \"" << cmd_dumper_prefix << "\"" << endl;
        ss << "Bypass bot actions: " << (bypass_bot_actions ? "True" : "False") << endl;
        ss << "Pick ticks[" << peek_ticks.size() << "]: ";
        for (const Tick &t : peek_ticks) ss << t << ", ";
        ss << endl;
        ss << "Main Loop quota: " << main_loop_quota << endl;
        ss << "Cmd Verbose: " << (cmd_verbose ? "True" : "False") << endl;
        ss << "Output file: " << output_file << endl;
        ss << "Output stream: " << (output_stream ? "Not Null" : "Null") << endl;
        ss << "Max ticks: " << max_tick << endl;
        ss << "Tick prompt n step: " << tick_prompt_n_step << endl;
        ss << "Save with binary format: " << (save_with_binary_format ? "True" : "False") << endl;
        ss << "Datafiles root: " << datafiles_root << endl;
        return ss.str();
    }
	void AdjustToDatafilesRoot() {
	    //cout << PrintInfo() << endl;
		if (datafiles_root.empty()){
			snapshot_dir = ".";
			replay_dir = ".";
		}
		else {
			/* datafiles_root is present
			 *  - if snapshot_prefix has contents, it means the
			 *  save_snapshot_prefix flag was used, which means we assume its a new
			 *  run we want to save so if there is already data in the specified
			 *  datafiles_dir, clean it out.
			 *  -adjust snapshot dir and replay dir
			 */
			if (!snapshot_prefix.empty()) {
				// New snapshot files coming in, so clean out datafiles_root
				// to make way for new replay and potentially shapshot data
				FileUtils::clean_directory(datafiles_root);
			}
			if (!FileUtils::is_valid_dir(datafiles_root)) {
				if (!FileUtils::ensure_directory_exists(datafiles_root)) {
					throw std::runtime_error(
							"The specified directory could not be created: "
									+ datafiles_root);
				}
			}
			snapshot_dir = datafiles_root + "/snapshot";
			if (!FileUtils::ensure_directory_exists(snapshot_dir)){
				throw std::runtime_error("Could not create directory " + snapshot_dir);
			}
			replay_dir   = datafiles_root + "/replay";
			if (!FileUtils::ensure_directory_exists(replay_dir)){
				throw std::runtime_error("Could not create directory " + replay_dir);
			}
            decision_dir = datafiles_root + "/decisions";
            if (!FileUtils::ensure_directory_exists(decision_dir)) {
                throw std::runtime_error(
                        "Could not create directory " + decision_dir);
            }
		}
	}
};

// A tick-based RTS game.
class RTSGame {
private:
    // Spectator
    unique_ptr<AI> _spectator;

    // AIs
    vector<unique_ptr<AI> > _bots;

    // Game Env.
    GameEnv _env;

    // Options.
    RTSGameOptions _options;

    // Receivers.
    CmdReceiver _cmd_receiver;

    // Next snapshot to load.
    int _snapshot_to_load;

    // Whether we pause the system.
    bool _paused;

    // Output file to save.
    bool _output_stream_owned;
    ostream *_output_stream;

private:
    // Dispatch commands received from gui.
    CmdReturn dispatch_cmds(const UICmd& cmd);

    // Move to certain point of a replay. 0.0 - 1.0
    bool move_to_tick(float percent);

    //OSU
    // Move to certain tick in replay
    bool move_to_tick_number(int tick_number);

    // Update simulation speed, used in gui.
    bool change_simulation_speed(float fraction);

    // Save and Load from a snapshot file.
    void save_snapshot(const string &filename) const;
    void load_snapshot(const string &filename);

    // Load a game from a state string.
    void load_from_string(const string &s);

public:
    // Initialize the game.
    explicit RTSGame(const RTSGameOptions &options);

    // Not a good design. Need to fix.
    CmdReceiver *GetCmdReceiver() { return &_cmd_receiver; }

    // Get game environment associated with rts game.
    const GameEnv &GetGameEnv() const { return _env; }

    // Reset game conditions
    void Reset();

    // Add and remove bot.
    void AddBot(AI *bot);
    void RemoveBot();
    void AddSpectator(AI *spectator);
    // Start the game.
    bool PrepareGame();
    PlayerId MainLoop(const std::atomic_bool *done = nullptr);
    // Step the game once and modify state string in place.
    PlayerId Step(int num_step, std::string *state);
    void save_to_string(string *s) const;
    ~RTSGame();
};

#endif
