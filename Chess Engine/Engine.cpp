#include "Engine.hpp"
#include <iostream>

std::vector<Engine::TTEntry> Engine::transpositionTable;

Engine::Engine(Board& b, int depth)
	: board(b), searchDepth(depth)
{
	transpositionTable.resize(TRANSPOSITION_TABLE_SIZE);
}

Move Engine::getBestMove() {
	if (board.getPlayer()) {
		vector<Move> moves = board.getMoves(true);

		double highest = -DBL_MAX;
		Move best;

		for (int i = 0; i < moves.size(); i++) {
			board.makeMove(moves[i]);
			double score = minimax(board, searchDepth - 1, -DBL_MAX, DBL_MAX);
            board.undoMove(moves[i]);
            if (score > highest) {
				highest = score;
				best = moves[i];
			}
		}
		return best;
	}
	else {
		vector<Move> moves = board.getMoves(false);

		double lowest = DBL_MAX;
		Move best;

		for (int i = 0; i < moves.size(); i++) {
			board.makeMove(moves[i]);
			double score = minimax(board, searchDepth - 1, -DBL_MAX, DBL_MAX);
            board.undoMove(moves[i]);
			if (score < lowest) {
				lowest = score;
				best = moves[i];
			}
		}
		return best;
	}
}

double Engine::minimax(Board& board, int depth, double alpha, double beta) {
    uint64_t posKey = board.getZobristKey();
    TTEntry& entry = transpositionTable[posKey & (TRANSPOSITION_TABLE_SIZE - 1)];

    if (entry.zobristKey == posKey && entry.depth >= depth) {
        // Position found in table with sufficient depth
        if (entry.flag == TTFlag::EXACT) return entry.score;
        if (entry.flag == TTFlag::LOWERBOUND) alpha = std::max(alpha, entry.score);
        if (entry.flag == TTFlag::UPPERBOUND) beta = std::min(beta, entry.score);
        if (alpha >= beta) return entry.score;
    }

    if (board.kingInCheck(!board.getPlayer())) {
        if (board.getPlayer()) {
            return alpha;
        }
        else {
            return beta;
        }
    }

    if (depth <= 0) {
        return evalBoard(board);
    }

    vector<Move> moves = board.getMoves(board.getPlayer());

    double originalAlpha = alpha;
    double bestScore = board.getPlayer() ? -DBL_MAX : DBL_MAX;

    for (const Move& move : moves) {
        board.makeMove(move);
        double score = minimax(board, depth - 1, alpha, beta);
        board.undoMove(move);

        if (board.getPlayer()) {
            bestScore = std::max(bestScore, score);
            alpha = std::max(alpha, score);
        }
        else {
            bestScore = std::min(bestScore, score);
            beta = std::min(beta, score);
        }

        if (beta <= alpha) break;
    }

    TTFlag flag;
    if (bestScore <= originalAlpha) {
        flag = TTFlag::UPPERBOUND;
    }
    else if (bestScore >= beta) {
        flag = TTFlag::LOWERBOUND;
    }
    else {
        flag = TTFlag::EXACT;
    }

    entry = { posKey, bestScore, depth, flag };

    return bestScore;
}

double Engine::evalBoard(Board& board) {
    // Material values (standard piece values in centipawns)
    const double PAWN_VALUE = 100.0;
    const double KNIGHT_VALUE = 320.0;
    const double BISHOP_VALUE = 330.0;
    const double ROOK_VALUE = 500.0;
    const double QUEEN_VALUE = 900.0;
    const double KING_VALUE = 20000.0;

    // Piece-Square tables for positional evaluation
    // These values encourage pieces to move to strategically advantageous squares
    // Values are for white's perspective - will be mirrored for black
    const double PAWN_POS[8][8] = {
        {0,  0,  0,  0,  0,  0,  0,  0},
        {50, 50, 50, 50, 50, 50, 50, 50},
        {10, 10, 20, 30, 30, 20, 10, 10},
        {5,  5, 10, 25, 25, 10,  5,  5},
        {0,  0,  0, 20, 20,  0,  0,  0},
        {5, -5,-10,  0,  0,-10, -5,  5},
        {5, 10, 10,-20,-20, 10, 10,  5},
        {0,  0,  0,  0,  0,  0,  0,  0}
    };

    const double KNIGHT_POS[8][8] = {
        {-50,-40,-30,-30,-30,-30,-40,-50},
        {-40,-20,  0,  0,  0,  0,-20,-40},
        {-30,  0, 10, 15, 15, 10,  0,-30},
        {-30,  5, 15, 20, 20, 15,  5,-30},
        {-30,  0, 15, 20, 20, 15,  0,-30},
        {-30,  5, 10, 15, 15, 10,  5,-30},
        {-40,-20,  0,  5,  5,  0,-20,-40},
        {-50,-40,-30,-30,-30,-30,-40,-50}
    };

    const double BISHOP_POS[8][8] = {
        {-20,-10,-10,-10,-10,-10,-10,-20},
        {-10,  0,  0,  0,  0,  0,  0,-10},
        {-10,  0,  5, 10, 10,  5,  0,-10},
        {-10,  5,  5, 10, 10,  5,  5,-10},
        {-10,  0, 10, 10, 10, 10,  0,-10},
        {-10, 10, 10, 10, 10, 10, 10,-10},
        {-10,  5,  0,  0,  0,  0,  5,-10},
        {-20,-10,-10,-10,-10,-10,-10,-20}
    };

    const double ROOK_POS[8][8] = {
        { 0,  0,  0,  0,  0,  0,  0,  0},
        { 5, 10, 10, 10, 10, 10, 10,  5},
        {-5,  0,  0,  0,  0,  0,  0, -5},
        {-5,  0,  0,  0,  0,  0,  0, -5},
        {-5,  0,  0,  0,  0,  0,  0, -5},
        {-5,  0,  0,  0,  0,  0,  0, -5},
        {-5,  0,  0,  0,  0,  0,  0, -5},
        { 0,  0,  0,  5,  5,  0,  0,  0}
    };

    const double QUEEN_POS[8][8] = {
        {-20,-10,-10, -5, -5,-10,-10,-20},
        {-10,  0,  0,  0,  0,  0,  0,-10},
        {-10,  0,  5,  5,  5,  5,  0,-10},
        { -5,  0,  5,  5,  5,  5,  0, -5},
        {  0,  0,  5,  5,  5,  5,  0, -5},
        {-10,  5,  5,  5,  5,  5,  0,-10},
        {-10,  0,  5,  0,  0,  0,  0,-10},
        {-20,-10,-10, -5, -5,-10,-10,-20}
    };

    const double KING_POS[8][8] = {
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-20,-30,-30,-40,-40,-30,-30,-20},
        {-10,-20,-20,-20,-20,-20,-20,-10},
        { 20, 20,  0,  0,  0,  0, 20, 20},
        { 20, 30, 10,  0,  0, 10, 30, 20}
    };

    double score = 0.0;

    // Evaluate each position on the board
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            bitboard currentSquare = Board::getBitboard(row, col);

            // White pieces (positive scores)
            if (currentSquare & board.getWhitePawns())
                score += PAWN_VALUE + PAWN_POS[row][col];
            else if (currentSquare & board.getWhiteKnights())
                score += KNIGHT_VALUE + KNIGHT_POS[row][col];
            else if (currentSquare & board.getWhiteBishops())
                score += BISHOP_VALUE + BISHOP_POS[row][col];
            else if (currentSquare & board.getWhiteRooks())
                score += ROOK_VALUE + ROOK_POS[row][col];
            else if (currentSquare & board.getWhiteQueens())
                score += QUEEN_VALUE + QUEEN_POS[row][col];
            else if (currentSquare & board.getWhiteKing())
                score += KING_VALUE + KING_POS[row][col];

            // Black pieces (negative scores)
            if (currentSquare & board.getBlackPawns())
                score -= PAWN_VALUE + PAWN_POS[7 - row][col];
            else if (currentSquare & board.getBlackKnights())
                score -= KNIGHT_VALUE + KNIGHT_POS[7 - row][col];
            else if (currentSquare & board.getBlackBishops())
                score -= BISHOP_VALUE + BISHOP_POS[7 - row][col];
            else if (currentSquare & board.getBlackRooks())
                score -= ROOK_VALUE + ROOK_POS[7 - row][col];
            else if (currentSquare & board.getBlackQueens())
                score -= QUEEN_VALUE + QUEEN_POS[7 - row][col];
            else if (currentSquare & board.getBlackKing())
                score -= KING_VALUE + KING_POS[7 - row][col];
        }
    }

    // Mobility evaluation (number of legal moves)
    vector<Move> whiteMoves = board.getMoves(true);
    vector<Move> blackMoves = board.getMoves(false);
    score += whiteMoves.size() * 10;
    score -= blackMoves.size() * 10;

    // King safety
    if (board.kingInCheck(true)) score -= 50;
    if (board.kingInCheck(false)) score += 50;

    return score;
}