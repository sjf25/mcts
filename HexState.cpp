#include "HexState.h"
#include <algorithm>
#include <cassert>

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
using namespace emscripten;
EMSCRIPTEN_BINDINGS(test_class_thing) {
	register_vector<int>("VectorInt");
	enum_<HexCell>("HexCell")
		.value("BLANK", HexCell::BLANK)
		.value("P1", HexCell::P1)
		.value("P2", HexCell::P2);
	class_<HexState>("HexState")
		.constructor<>()
		.function("previousPlayer", &HexState::previous_player)
		.function("options", &HexState::options)
		.function("makeMove", &HexState::make_move)
		.function("score", &HexState::score)
		.function("gameDone", &HexState::game_done);
}
#endif

HexCell HexState::winner_helper(HexCell target, bool seen[][BOARD_SIZE], int i, int j,
		bool maximizing_i) const {
	assert(i >= 0 && j >= 0);
	assert(i < BOARD_SIZE && j < BOARD_SIZE);
	if(seen[i][j])
		return HexCell::BLANK;
	if(board[i][j] != target)
		return HexCell::BLANK;
	seen[i][j] = true;
	if(maximizing_i && i == BOARD_SIZE - 1)
		return target;
	else if(!maximizing_i && j == BOARD_SIZE - 1)
		return target;
	for(int di = -1; di <= 1; di++) {
		for(int dj = -1; dj <=1; dj++) {
			if(di == dj)
				continue;
			if(i + di < 0 || i + di >= BOARD_SIZE || j + dj < 0
					|| j + dj >= BOARD_SIZE)
				continue;
			HexCell option = winner_helper(target, seen, i+di, j+dj, maximizing_i);
			if(option != HexCell::BLANK)
				return option;
		}
	}
	return HexCell::BLANK;
}

HexCell HexState::winner() const {
	bool seen[BOARD_SIZE][BOARD_SIZE] = {};
	for(int i = 0; i < BOARD_SIZE; i++) {
		HexCell possible_winner = winner_helper(HexCell::P1, seen, 0, i, true);
		if(possible_winner != HexCell::BLANK)
			return possible_winner;
	}
	for(int i = 0; i < BOARD_SIZE; i++)
		for(int j = 0; j < BOARD_SIZE; j++)
			seen[i][j] = false;
	for(int i = 0; i < BOARD_SIZE; i++) {
		HexCell possible_winner = winner_helper(HexCell::P2, seen, i, 0, false);
		if(possible_winner != HexCell::BLANK)
			return possible_winner;
	}
	return HexCell::BLANK;
}

HexState::HexState() {
	for(int i = 0; i < BOARD_SIZE; i++)
		for(int j = 0; j < BOARD_SIZE; j++)
			board[i][j] = HexCell::BLANK;
}

HexCell HexState::previous_player() const {
	return turn == HexCell::P1 ? HexCell::P2 : HexCell::P1;
}

std::vector<int> HexState::options() const {
	std::vector<int> result;
	for(int i = 0; i < BOARD_SIZE; i++) {
		for(int j = 0; j < BOARD_SIZE; j++) {
			if(board[i][j] == HexCell::BLANK) {
				result.push_back(i*BOARD_SIZE + j);
			}
		}
	}
	return result;
}

void HexState::make_move(int move_to_make) {
	int i = move_to_make/BOARD_SIZE;
	int j = move_to_make%BOARD_SIZE;
	assert(i >= 0 && j >= 0);
	assert(i < BOARD_SIZE && j < BOARD_SIZE);
	board[i][j] = turn;
	turn = turn == HexCell::P1 ? HexCell::P2 : HexCell::P1;
}

void HexState::make_rand_move(std::mt19937& rand_gen) {
	auto ops = options();
	assert(ops.size() > 0);
	std::uniform_int_distribution<> dist(0, ops.size()-1);
	int idx = ops[dist(rand_gen)];
	make_move(idx);
}

double HexState::score(HexCell player) const {
	HexCell w = winner();
	if(w == HexCell::BLANK)
		return 0.5;
	return w == player ? 1.0 : 0.0;
}

// the game can never end in a draw
bool HexState::game_done() const {
	assert(!(options().size() == 0) || (winner() != HexCell::BLANK));
	return winner() != HexCell::BLANK;
}
