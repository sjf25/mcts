#include <cstdlib>
#include <random>

#include "ttt.h"
#include "HexState.h"
#include "Node.h"
//#include "ChessState.h"

template <typename GameState>
Node<GameState>* mcts(const GameState& state, int num_iter) {
	std::mt19937 rand_gen(std::random_device{}());
	Node<GameState>* root = new Node<GameState>(state, -1, nullptr, rand_gen);
	for(int it = 0; it < num_iter; it++) {
		Node<GameState>* current_node = root;
		GameState current_state = state;
		
		while(!current_node->has_avail_move() && current_node->has_children()){
			Node<GameState>* new_node = current_node->select_UCB1();
			move_t next_move = new_node->prev_move;
			current_state.make_move(next_move);
			current_node = new_node;
		}

		if(current_node->has_avail_move()) {
			move_t next_move = current_node->random_new_move();
			current_state.make_move(next_move);
			current_node = current_node->add_child(next_move, current_state);
		}

		while(!current_state.game_done()) {
			current_state.make_rand_move(rand_gen);
		}

		while(current_node != nullptr) {
			current_node->update_score(current_state);
			current_node = current_node->parent;
		}
	}
	return root;
}

void ttt_play_human() {
	TTTState game_state;
	bool human_turn = true;
	while(!game_state.game_done()) {
		std::cout << game_state;
		if(human_turn) {
			int move;
			std::cout << "enter your move: ";
			std::cin >> move;
			game_state.make_move(move);
		}
		else {
			Node<TTTState>* node = mcts(game_state, 10000);

			int best_move = node->best_move();
			game_state.make_move(best_move);
			delete node;
		}
		human_turn ^= true;
		std::cout << "--------------------------------\n";
	}
	std::cout << game_state;
}

#if 0
void chess_play_game() {
	ChessState game_state;
	//bool my_turn = false;
	bool my_turn = true;
	while(!game_state.game_done()) {
		//chess_print(game_state.state);
		std::cout << chess_fen(game_state.state) << std::endl;
		std::cout << chess_pretty(game_state.state) << std::endl;
		//std::cout << chess_moves_pretty(game_state.state) << std::endl;
		if(my_turn) {
			//Node<ChessState>* node = mcts(game_state, 10000);
			Node<ChessState>* node = mcts(game_state, 100000);
			//Node<ChessState>* node = mcts(game_state, 100000);


			int best_move = node->best_move();
			game_state.make_move(best_move);
			delete node;
		}
		else {
			chess_ai_play_move(game_state.state);
		}

		my_turn ^= true;
	
		//std::cout << "--------------------------------\n";
	}
	//chess_print(game_state.state);
	std::cout << chess_fen(game_state.state) << std::endl;
	std::cout << chess_pretty(game_state.state) << std::endl;
	std::cout << "score: " << chess_score(game_state.state, 1) << std::endl;
	chess_print_winner(game_state.state);
}
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
using namespace emscripten;
EMSCRIPTEN_BINDINGS(mcts_thing) {
	register_map<move_t, int>("MoveCountMap");
	class_<Node<HexState>>("MCTSNode")
		.function("bestMove", &Node<HexState>::best_move)
		.function("moveCounts", &Node<HexState>::move_counts);
	function("mcts", &mcts<HexState>, allow_raw_pointers());
}
#endif

int main(int argc, char** argv) {
	//Node<TTTState>* n = mcts<TTTState>(TTTState{}, 1000);
	//std::cout << n->current_player << std::endl;
	//std::cout << n->children[0]->current_player << std::endl;
	//delete n;
	ttt_play_human();

	//Node<HexState>* n = mcts<HexState>(HexState{}, 10000);
	//delete n;

	//Node<ChessState>* n = mcts<ChessState>(ChessState{}, 50000);
	//delete n;
	//chess_play_game();
}
