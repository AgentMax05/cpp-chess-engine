#pragma once

#include "Types.hpp"

#include <vector>
using std::vector;

class Storage {
private:
	static bitboard pawnMaskWhiteTake[8][8];
	static bitboard pawnMaskBlackTake[8][8];

	static bitboard pawnMaskWhite[8][8];
	static bitboard pawnMaskBlack[8][8];

	static bitboard knightMask[8][8];
	static bitboard kingMask[8][8];

	// stores blocker masks
	static bitboard rookBlockers[8][8];
	static bitboard bishopBlockers[8][8];

	// stores corresponding right shifts
	static int rookShifts[8][8];
	static int bishopShifts[8][8];

	static vector<vector<bitboard>> rookDB;
	static vector<vector<bitboard>> bishopDB;

	static bitboard generateRookMoveboard(int row, int col, bitboard blockerboard);
	static bitboard generateBishopMoveboard(int row, int col, bitboard blockerboard);

	static void generateMoves();
	static void generateShifts();

	static bitboard getPawnMovesTake(int row, int col, bool white);
	static bitboard getPawnMoves(int row, int col, bool white);
	static bitboard getRookMoves(int row, int col);
	static bitboard getKnightMoves(int row, int col);
	static bitboard getBishopMoves(int row, int col);
	static bitboard getKingMoves(int row, int col);

	static uint64_t getIndex(int row, int col, Piece p, bitboard blocker);
	static bitboard generateBlockerboard(uint64_t index, bitboard blockermask);
	static void initDB();

public:
	static void init();

	static const bitboard EDGES = 0b1111111110000001100000011000000110000001100000011000000111111111;
	static int popcount(bitboard b);

	static bitboard getMoveboard(int row, int col, bitboard enemy, bitboard friendly, Piece piece, bool white);
	static bitboard getAttackingMoveboard(int row, int col, bitboard enemy, bitboard friendly, Piece piece, bool white);

	static const uint64_t rookMagics[8][8];
	static const uint64_t bishopMagics[8][8];

	static bitboard getLSB(bitboard b);
	static int bitboardToSqIndex(bitboard b);
};
