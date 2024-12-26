#pragma once

#include "Types.hpp"
#include <string>
#include <vector>
#include <unordered_map>

using std::vector;

class Board {
private:
	bitboard bPawn = 0;
	bitboard wPawn = 0;

	bitboard bBishop = 0;
	bitboard wBishop = 0;

	bitboard bKnight = 0;
	bitboard wKnight = 0;

	bitboard bRook = 0;
	bitboard wRook = 0;

	bitboard bKing = 0;
	bitboard wKing = 0;

	bitboard bQueen = 0;
	bitboard wQueen = 0;

	bitboard attackingW = 0;
	bitboard attackingB = 0;

	bitboard allWhite = 0;
	bitboard allBlack = 0;

	vector<bitboard*> whiteBitboards;
	vector<bitboard*> blackBitboards;

	vector<Piece> pieceMap = { Piece::Pawn, Piece::Bishop, Piece::Knight, Piece::Rook, Piece::King, Piece::Queen };
	
	std::unordered_map<Piece, bitboard*> wBitboardLookup = {
		{ Piece::Pawn, &wPawn },
		{ Piece::Bishop, &wBishop },
		{ Piece::Knight, &wKnight },
		{ Piece::Rook, &wRook },
		{ Piece::King, &wKing },
		{ Piece::Queen, &wQueen }
	};

	std::unordered_map<Piece, bitboard*> bBitboardLookup = {
		{ Piece::Pawn, &bPawn },
		{ Piece::Bishop, &bBishop },
		{ Piece::Knight, &bKnight },
		{ Piece::Rook, &bRook },
		{ Piece::King, &bKing },
		{ Piece::Queen, &bQueen }
	};

	bool whiteTurn;
	int numMoves;
	void setAttacking();
	void setTotalBoards();
public:
	Board(std::string fen);
	//Board(const Board& other);

	std::string print_board();

	void makeMove(Move move);
	void undoMove(Move move);

	vector<Move> getMoves(bool white);

	bool kingInCheck(bool white);

	inline bool getPlayer() { return whiteTurn; }
	inline void togglePlayer() { whiteTurn = !whiteTurn; }
	

	// static helpers

	static bitboard getBitboard(int row, int col);
	static bitboard getBitboard(int sqIndex);

	static std::string print_bitboard(bitboard b);
	static int getSquareIndex(int row, int col);
	static std::pair<int, int> getRowCol(int sqIndex);
	static bitboard stringToBitboard(std::string s);
	
	static bool oneAtBitboard(int row, int col, bitboard b);

	static const std::unordered_map<Piece, std::string> pieceToString;

	inline bitboard getAllBlack() const { return allBlack; }
	inline bitboard getAllWhite() const { return allWhite; }

	inline bitboard getWhitePawns() const { return wPawn; }
	inline bitboard getBlackPawns() const { return bPawn; }
	inline bitboard getWhiteKnights() const { return wKnight; }
	inline bitboard getBlackKnights() const { return bKnight; }
	inline bitboard getWhiteBishops() const { return wBishop; }
	inline bitboard getBlackBishops() const { return bBishop; }
	inline bitboard getWhiteRooks() const { return wRook; }
	inline bitboard getBlackRooks() const { return bRook; }
	inline bitboard getWhiteQueens() const { return wQueen; }
	inline bitboard getBlackQueens() const { return bQueen; }
	inline bitboard getWhiteKing() const { return wKing; }
	inline bitboard getBlackKing() const { return bKing; }
};