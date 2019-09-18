#ifndef NODE_H
#define NODE_H

#include <vector>
#include <map>
#include <random>
#include <cassert>
#include <algorithm>

typedef int move_t;
typedef int player_t;

template <typename GameState>
struct Node {
	double bias_param;

	move_t prev_move;
	int num_visited = 0;
	double num_wins = 0;
	std::vector<Node*> children;
	std::vector<move_t> avail_moves;
	Node* parent;
	decltype(GameState().previous_player()) previous_player;

	std::mt19937 rand_gen;

	Node<GameState>(const GameState& state, move_t prev_move, Node* parent, std::mt19937& rand_gen)
		: rand_gen(rand_gen), bias_param(std::sqrt(2.0)),
		prev_move(prev_move), parent(parent), children() {
		avail_moves = state.options();
		previous_player = state.previous_player();
	}

	bool has_avail_move() {
		return avail_moves.size() > 0;
	}

	bool has_children() {
		return children.size() > 0;
	}

	move_t random_new_move() {
		std::uniform_int_distribution<> dist(0, avail_moves.size()-1);
		return avail_moves[dist(rand_gen)];
	}
	
	double value_UCB1(Node* child) {
		return (double)child->num_wins/(double)child->num_visited + bias_param *
			std::sqrt(std::log((double)num_visited) / (double)child->num_visited);
	}

	Node<GameState>* select_UCB1() {
		return *std::max_element(children.begin(), children.end(),
				[this](Node* node_1, Node* node_2) {
					return value_UCB1(node_1) < value_UCB1(node_2);
				});
	}
	Node<GameState>* add_child(move_t new_move, const GameState& state) {
		auto move_position = std::find(avail_moves.begin(), avail_moves.end(),
				new_move);
		assert(move_position != avail_moves.end());

		avail_moves.erase(move_position);
		Node<GameState>* child = new Node<GameState>(state, new_move, this, this->rand_gen);
		children.push_back(child);

		assert(std::find(avail_moves.begin(), avail_moves.end(), new_move) ==
				avail_moves.end());
		return child;
	}
	void update_score(const GameState& state) {
		num_visited++;
		num_wins += state.score(previous_player);
	}

	~Node<GameState>() {
		for(int i = 0; i < children.size(); i++)
			delete children[i];
	}
	Node<GameState> best_child() {
		int best_child = 0;
		int best_num_visited = children[0]->num_visited;

		for(int i = 1; i < children.size(); i++) {
			if(children[i]->num_visited > best_num_visited) {
				best_num_visited = children[i]->num_visited;
				best_child = i;
			}
		}
		Node<GameState>* best_child_node = children[best_child];
		children.erase(children.begin() + best_child);
		return best_child_node;
	}
	move_t best_move() {
		int best_child = 0;
		int best_num_visited = children[0]->num_visited;

		for(int i = 1; i < children.size(); i++) {
			if(children[i]->num_visited > best_num_visited) {
				best_num_visited = children[i]->num_visited;
				best_child = i;
			}
		}
		int next_move = children[best_child]->prev_move;
		return next_move;
	}
	std::map<move_t, int> move_counts() {
		std::map<move_t, int> result;
		for(Node* child : children) {
			result[child->prev_move] = child->num_visited;
		}
		return result;
	}
};

#endif
