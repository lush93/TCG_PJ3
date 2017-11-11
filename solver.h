#pragma once
#include <iostream>
#include <string>
#include <cmath>
#include "board2x3.h"
#include "type.h"

class solver {
public:
	typedef float value_t;

public:
	class answer {
	public:
		answer(value_t value) : value(value) {}
	    friend std::ostream& operator <<(std::ostream& out, const answer& ans) {
	    	return out << (std::isnan(ans.value) ? -1 : ans.value);
		}
	public:
		const value_t value;
	};

public:
	float get_set_table(const board2x3& state, bool is_before, float value){
		std::vector<float>* table = is_before ? &table_before : &table_after;
		int idx = 0;
		for(int i=0; i<6; i++)
	    	idx += state(i) * std::pow(33, i);
		
		if(value != -1){
			(*table)[idx] = value;
/*
			idx = 0;
			for(int i=2, j=0; i>=0; i--){
				if(state(i)==0) continue;
				idx += state(i) * std::pow(33, 2 - j++);
			}
			for(int i=3; i<6; i++)
	    		idx += state(i) * std::pow(33, i);
			(*table)[idx] = value;

			idx = 0;
			for(int i=0; i<3; i++)
				idx += state(i) * std::pow(33, i+3);
			for(int i=3; i<6; i++)
	    		idx += state(i) * std::pow(33, i-3);
			(*table)[idx] = value;

			idx = 0;
			for(int i=2, j=0; i>=0; i--){
				if(state(i)==0) continue;
				idx += state(i) * std::pow(33, 5 - j++);
			}
			for(int i=3; i<6; i++)
	    		idx += state(i) * std::pow(33, i-3);
			(*table)[idx] = value;*/
		}

		return (*table)[idx];
	}

	void build_tree(const board2x3& after){
		int space[] = { 0, 1, 2, 3, 4, 5 };
		int empty= 0;
		for (int pos : space) if (after(pos) == 0) empty++;
		if(empty > 4){
			for (int pos : space) {
				if (after(pos) != 0) continue;
				for(int tile = 1; tile <= 2 ;tile++){
					action move = action::place(tile, pos);
					board2x3 b = after;
					move.apply(b);
					std::cout << b << std::endl;
					build_tree(b);
				}
			}
		}
		else ecpected_max(after, true);
	}

	float ecpected_max(const board2x3& state, bool is_before){
		float value = get_set_table(state, is_before, -1);
		if(value > -1) return value;
		else value = 0;

		if(is_before){
			int opcode[] = { 0, 1, 2, 3 };
			for (int op : opcode) {
				board2x3 b = state;
				float r = b.move(op);
				if (r > -1){
					float value_next = r +  ecpected_max(b, false);
					if(value_next > value) value = value_next;
				}
			}
		}
		else{
			int space[] = { 0, 1, 2, 3, 4, 5 };
			float empty= 0;
			for (int pos : space) if (state(pos) == 0) empty++;
			if(empty > 0){
				for (int pos : space) {
					if (state(pos) != 0) continue;
					for(int tile = 1; tile <= 2 ;tile++){
						action move = action::place(tile, pos);
						board2x3 b = state;
						move.apply(b);
						float prob = (tile == 1) ? 0.9 : 0.1;
						value += (1/empty) * prob * ecpected_max(b, true);
					}
				}
			}
		}
		
		get_set_table(state, is_before, value);
		//std::cout << value << std::endl;
		return value;
	}

	solver(const std::string& args) {
		// TODO: explore the tree and save the result
		for(int i=0; i<std::pow(33, 6);i++){
			table_before.emplace_back(-1);
			table_after.emplace_back(-1);
		}
		std::cout << "solver is initialized." << std::endl;
		board2x3 game;
		build_tree(game);
		std::cout << "solver is finished." << std::endl;
	}

	answer solve2x3(const board2x3& state, state_type type = state_type::before) {
		// TODO: find the answer in the lookup table and return it
		return get_set_table(state, type.is_before(), -1);
	}

private:
	// TODO: place your transposition table here
	std::vector<float> table_before;
	std::vector<float> table_after;
};
