#pragma once

#include "Board.hpp"
#include "Storage.hpp"

class Engine {
private:
	Board& board;
	int searchDepth;
	
	static double minimax(Board& b, int depth, double alpha, double beta);
	static double evalBoard(Board& b);
public:
	Engine(Board& b, int depth);
	Move getBestMove();
	
};