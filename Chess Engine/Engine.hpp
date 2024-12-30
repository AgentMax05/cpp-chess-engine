#pragma once

#include "Board.hpp"
#include "Storage.hpp"

class Engine {
private:
	Board& board;
	int searchDepth;
	
	static double minimax(Board& b, int depth, double alpha, double beta);
	static double evalBoard(Board& b);

	static const int TRANSPOSITION_TABLE_SIZE = 1 << 20;

	enum class TTFlag {
		EXACT,
		LOWERBOUND,
		UPPERBOUND
	};

	struct TTEntry {
		uint64_t zobristKey;
		double score;
		int depth;
		TTFlag flag;
	};

	static std::vector<TTEntry> transpositionTable;

public:
	Engine(Board& b, int depth);
	Move getBestMove();
	
};