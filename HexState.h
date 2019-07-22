#ifndef HEX_STATE_H
#define HEX_STATE_H

#include <random>

enum class HexCell {
	BLANK,
	P1,
	P2
};

class HexState {
	static constexpr int BOARD_SIZE = 11;
	HexCell board[BOARD_SIZE][BOARD_SIZE];
	HexCell turn = HexCell::P1;
	
	HexCell winner_helper(HexCell, bool[][BOARD_SIZE], int, int, bool) const;

	HexCell winner() const;
public:
	HexState();

	//HexCell current_player() const;
	HexCell previous_player() const;

	std::vector<int> options() const;

	void make_move(int move_to_make);

	void make_rand_move(std::mt19937&);

	double score(HexCell) const;

	bool game_done() const;

};

#endif
