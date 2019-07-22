#ifndef TTT_H
#define TTT_H

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>

class TTTState {
	enum Cell { X, O, BLANK };
	Cell turn = X;
	Cell board[9];
public:
	TTTState() {
		for(int i = 0; i < 9; i++)
			board[i] = BLANK;
	}
	bool three_same(Cell cell_val, int i, int j, int k) const {
		return cell_val == board[i] && board[i] == board[j]
			&& board[i] == board[k];
	}
	bool find_three(Cell cell_val) const {
		for(int i = 0; i <= 6; i += 3)
			if(three_same(cell_val, i, i+1, i+2))
				return true;
		for(int i = 0; i <= 2; i++)
			if(three_same(cell_val, i, i+3, i+6))
				return true;
		if(three_same(cell_val, 0, 4, 8))
			return true;
		if(three_same(cell_val, 2, 4, 6))
			return true;
		return false;
	}
	bool game_done() const {
		return std::all_of(board, board+9, [](Cell c){return c != BLANK;}) ||
			find_three(X) || find_three(O);
	}
	Cell winner() const {
		if(find_three(X))
			return X;
		if(find_three(O))
			return O;
		return BLANK;
	}
	double score(Cell player) const {
		Cell w = winner();
		if(w == BLANK)
			return 0.5;
		return w == player ? 1.0 : 0.0;
	}
	std::vector<int> options() const {
		std::vector<int> result;
		for(int i = 0; i < 9; i++)
			if(board[i] == BLANK)
				result.push_back(i);
		return result;
	}
	void make_move(int idx) {
		assert(board[idx] == BLANK);
		board[idx] = turn;
		turn = turn == X ? O : X;
	}
	void make_rand_move(std::mt19937& rand_gen) {
		auto ops = options();
		std::uniform_int_distribution<> dist(0, ops.size()-1);
		int idx = ops[dist(rand_gen)];
		make_move(idx);
	}
	/*
	Cell current_player() const {
		return turn;
	}
	*/
	Cell previous_player() const {
		return turn == X ? O : X;
	}
	static char cell_to_char(Cell c) {
		assert(c == X || c == O || c == BLANK);
		if(c == X)
			return 'X';
		if(c == O)
			return 'O';
		return ' ';
	}
	friend std::ostream& operator<<(std::ostream& os, const TTTState& state) {
		for(int i = 0; i < 3; i++) {
			os << cell_to_char(state.board[3*i]) << " | "
				<< cell_to_char(state.board[3*i+1]) << " | "
				<< cell_to_char(state.board[3*i+2]) << std::endl;
			if(i != 2)
				os << "---------\n";
		}
		os << std::endl;
		return os;
	}
};

#endif
