#ifndef CHECKERS_STATE_H
#define CHECKERS_STATE_H

#include <array>
#include <cassert>

enum CellOwner {
	EMPTY,
	P1_MAN,
	P2_MAN,
};

class CheckersState {
	struct Cell {
		CellOwner owner;
		bool is_king;
	};
	std::array<Cell, 32> board;
	CellOwner turn = P1;

	bool valid_position(int idx) {
		return idx >= 0 && idx < board.size();
	}

	void add_cell_moves(int cell_idx, std::vector<int>& all_moves) {
		assert(board[cell_idx].owner != EMPTY);
		int multiplier = board[cell_idx].owner == P1 ? 1 : -1;
		//if(valid_position(cell_idx + 4*multiplier) &&
	}
public:
	CheckersState() {
		board.fill(EMPTY);
	}
	CellOwner current_player() const { return turn; }
	std::vector<int> options() const {
		std::vector<int> all_options;
		for(int i = 0; i < board.size(); i++) {
			if(turn != board[i].owner)
				continue;
		}
	}
};

#endif
