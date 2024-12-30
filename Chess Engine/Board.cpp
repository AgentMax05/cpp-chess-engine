#include "Board.hpp"
#include "Storage.hpp"
#include <cmath>
#include <iostream>
#include <sstream>
#include <algorithm>

uint64_t Board::zobristTable[12][64];
uint64_t Board::zobristCastle[4];
uint64_t Board::zobristEnPassant[8];
uint64_t Board::zobristSideToMove;

Board::Board(std::string fen)
	: whiteTurn(true)
{
	whiteBitboards = { &wPawn, &wBishop, &wKnight, &wRook, &wKing, &wQueen };
	blackBitboards = { &bPawn, &bBishop, &bKnight, &bRook, &bKing, &bQueen };

	int row = 0, col = 0;
	for (int i = 0; i < fen.size(); i++) {
		char c = fen.at(i);

		if (c == '/') {
			row++;
			col = 0;
		}
		else if (isdigit(c)) {
			int num = c - '0';
			col += num;
		}
		else {
			bitboard* toChange = nullptr;
			bool upper = isupper(c);

			switch (tolower(c)) {
			case 'r':
				toChange = upper ? &wRook : &bRook;
				break;
			case 'p':
				toChange = upper ? &wPawn : &bPawn;
				break;
			case 'b':
				toChange = upper ? &wBishop : &bBishop;
				break;
			case 'n':
				toChange = upper ? &wKnight : &bKnight;
				break;
			case 'q':
				toChange = upper ? &wQueen : &bQueen;
				break;
			case 'k':
				toChange = upper ? &wKing : &bKing;
				break;
			}

			if (toChange) {
				*toChange |= getBitboard(row, col);
			}
			col++;
		}
	}

	setTotalBoards();
	setAttacking();
	setZobristKey();
}

//Board::Board(const Board& other)
//	: wPawn(other.wPawn)
//	, wBishop(other.wBishop)
//	, wKnight(other.wKnight)
//	, wRook(other.wRook)
//	, wKing(other.wKing)
//	, wQueen(other.wQueen)
//	, bPawn(other.bPawn)
//	, bBishop(other.bBishop)
//	, bKnight(other.bKnight)
//	, bRook(other.bRook)
//	, bKing(other.bKing)
//	, bQueen(other.bQueen)
//	, attackingW(other.attackingW)
//	, attackingB(other.attackingB)
//	, allWhite(other.allWhite)
//	, allBlack(other.allBlack)
//	, whiteTurn(other.whiteTurn)
//	, numMoves(other.numMoves)
//	, pieceMap(other.pieceMap)
//{
//	// Initialize the bitboard pointer vectors with pointers to our own bitboards
//	whiteBitboards = { &wPawn, &wBishop, &wKnight, &wRook, &wKing, &wQueen };
//	blackBitboards = { &bPawn, &bBishop, &bKnight, &bRook, &bKing, &bQueen };
//
//	// Set up the lookup maps with pointers to our own bitboards
//	wBitboardLookup = {
//		{ Piece::Pawn, &wPawn },
//		{ Piece::Bishop, &wBishop },
//		{ Piece::Knight, &wKnight },
//		{ Piece::Rook, &wRook },
//		{ Piece::King, &wKing },
//		{ Piece::Queen, &wQueen }
//	};
//
//	bBitboardLookup = {
//		{ Piece::Pawn, &bPawn },
//		{ Piece::Bishop, &bBishop },
//		{ Piece::Knight, &bKnight },
//		{ Piece::Rook, &bRook },
//		{ Piece::King, &bKing },
//		{ Piece::Queen, &bQueen }
//	};
//}

std::string Board::print_bitboard(bitboard bmap) {
	std::string result = "";
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			if ((bmap & getBitboard(row, col)) != 0) {
				result += "1";
			}
			else {
				result += "0";
			}
		}
		result += "\n";
	}
	return result;
}

std::string Board::print_board() {
	std::string result = "";

	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			if (oneAtBitboard(row, col, wPawn)) {
				result += "P";
			}
			else if (oneAtBitboard(row, col, bPawn)) {
				result += "p";
			}
			else if (oneAtBitboard(row, col, wBishop)) {
				result += "B";
			}
			else if (oneAtBitboard(row, col, bBishop)) {
				result += "b";
			}
			else if (oneAtBitboard(row, col, wKnight)) {
				result += "N";
			}
			else if (oneAtBitboard(row, col, bKnight)) {
				result += "n";
			}
			else if (oneAtBitboard(row, col, wRook)) {
				result += "R";
			}
			else if (oneAtBitboard(row, col, bRook)) {
				result += "r";
			}
			else if (oneAtBitboard(row, col, wKing)) {
				result += "K";
			}
			else if (oneAtBitboard(row, col, bKing)) {
				result += "k";
			}
			else if (oneAtBitboard(row, col, wQueen)) {
				result += "Q";
			}
			else if (oneAtBitboard(row, col, bQueen)) {
				result += "q";
			}
			else {
				result += ".";
			}
		}
		result += "\n";
	}
	return result;
}

int Board::getSquareIndex(int row, int col) {
	return row * 8 + col;
}

std::pair<int, int> Board::getRowCol(int sqIndex) {
	return std::make_pair(sqIndex / 8, sqIndex % 8);
}

bool Board::oneAtBitboard(int row, int col, bitboard b) {
	return ((b & (getBitboard(row, col))) != 0);
}

bitboard Board::stringToBitboard(std::string str) {
	bitboard result = 0;

	std::stringstream ss(str);
	std::string line;
	int row = 0;

	while (std::getline(ss, line)) {
		for (int col = 0; col < line.length(); col++) {
			if (line.at(col) == '1') {
				result |= getBitboard(row, col);
			}
		}
		row++;
	}

	return result;
}

void Board::setAttacking() {
	attackingW = 0;
	attackingB = 0;

	for (int i = 0; i < whiteBitboards.size(); i++) {
		bitboard current = *whiteBitboards[i];
		while (current) {
			bitboard lsb = Storage::getLSB(current);
			int sqIndex = Storage::bitboardToSqIndex(lsb);
			int row = sqIndex / 8;
			int col = sqIndex % 8;
			attackingW |= Storage::getAttackingMoveboard(row, col, allBlack, allWhite, pieceMap[i], true);

			current ^= lsb;
		}

		current = *blackBitboards[i];
		while (current) {
			bitboard lsb = Storage::getLSB(current);
			int sqIndex = Storage::bitboardToSqIndex(lsb);
			int row = sqIndex / 8;
			int col = sqIndex % 8;
			attackingB |= Storage::getAttackingMoveboard(row, col, allWhite, allBlack, pieceMap[i], false);

			current ^= lsb;
		}
	}
}

void Board::setTotalBoards() {
	allWhite = 0;
	allBlack = 0;

	for (int i = 0; i < whiteBitboards.size(); i++) {
		allWhite |= *(whiteBitboards[i]);
		allBlack |= *(blackBitboards[i]);
	}
}

int Board::pieceToIndex(Piece p) {
	return static_cast<int>(p);
}

void Board::makeMove(Move move) {
	if (move.white) {
		allWhite ^= (move.from | move.to);
		*(wBitboardLookup[move.piece]) ^= (move.from | move.to);

		zobristHash ^= zobristTable[pieceToIndex(move.piece)][Storage::bitboardToSqIndex(move.from)];
		zobristHash ^= zobristTable[pieceToIndex(move.piece)][Storage::bitboardToSqIndex(move.to)];

		if (move.isCapture) {
			allBlack &= ~move.to;

			*(bBitboardLookup[move.capturedPiece]) ^= move.to;
			zobristHash ^= zobristTable[pieceToIndex(move.capturedPiece) + 6][Storage::bitboardToSqIndex(move.to)];
		}
	}
	else {
		allBlack ^= (move.from | move.to);
		*(bBitboardLookup[move.piece]) ^= (move.from | move.to);

		zobristHash ^= zobristTable[pieceToIndex(move.piece) + 6][Storage::bitboardToSqIndex(move.from)];
		zobristHash ^= zobristTable[pieceToIndex(move.piece) + 6][Storage::bitboardToSqIndex(move.to)];

		if (move.isCapture) {
			allWhite &= ~move.to;

			*(wBitboardLookup[move.capturedPiece]) ^= move.to;
			zobristHash ^= zobristTable[pieceToIndex(move.capturedPiece)][Storage::bitboardToSqIndex(move.to)];
		}
	}

	togglePlayer();
	setAttacking();
}

void Board::undoMove(Move move) {
	if (move.white) {
		allWhite ^= (move.from | move.to);
		*(wBitboardLookup[move.piece]) ^= (move.from | move.to);

		zobristHash ^= zobristTable[pieceToIndex(move.piece)][Storage::bitboardToSqIndex(move.from)];
		zobristHash ^= zobristTable[pieceToIndex(move.piece)][Storage::bitboardToSqIndex(move.to)];

		if (move.isCapture) {
			allBlack |= move.to;

			*(bBitboardLookup[move.capturedPiece]) |= move.to;
			zobristHash ^= zobristTable[pieceToIndex(move.capturedPiece) + 6][Storage::bitboardToSqIndex(move.to)];
		}
	}
	else {
		allBlack ^= (move.from | move.to);
		*(bBitboardLookup[move.piece]) ^= (move.from | move.to);

		zobristHash ^= zobristTable[pieceToIndex(move.piece) + 6][Storage::bitboardToSqIndex(move.from)];
		zobristHash ^= zobristTable[pieceToIndex(move.piece) + 6][Storage::bitboardToSqIndex(move.to)];

		if (move.isCapture) {
			allWhite |= move.to;

			*(wBitboardLookup[move.capturedPiece]) |= move.to;
			zobristHash ^= zobristTable[pieceToIndex(move.capturedPiece)][Storage::bitboardToSqIndex(move.to)];
		}
	}

	togglePlayer();
	setAttacking();
}

bool compareMoves(Move& a, Move& b) {
	return a.getScore() > b.getScore();
}

vector<Move> Board::getMoves(bool white) {
	vector<Move> moves = {};

	if (white) {
		for (int i = 0; i < pieceMap.size(); i++) {
			bitboard current = *(whiteBitboards[i]);
			while (current) {
				bitboard lsb = Storage::getLSB(current);
				int sqIndex = Storage::bitboardToSqIndex(lsb);
				int row = sqIndex / 8;
				int col = sqIndex % 8;

				bitboard moveboard = Storage::getMoveboard(row, col, allBlack, allWhite, pieceMap[i], true);

				while (moveboard) {
					bitboard mlsb = Storage::getLSB(moveboard);
					Piece capture = Piece::None;

					if ((mlsb & allBlack) != 0) {
						for (int i = 0; i < pieceMap.size(); i++) {
							if ((*(blackBitboards[i]) & mlsb) != 0) {
								capture = pieceMap[i];
								break;
							}
						}
					}

					Move newMove = { lsb, mlsb, pieceMap[i], true, (mlsb & allBlack) != 0, capture };
					
					// check if move puts king in check
					//makeMove(newMove);
					//if (!kingInCheck(true)) {
					moves.push_back(newMove);
					//}
					//undoMove(newMove);

					moveboard ^= mlsb;
				}

				current ^= lsb;
			}
		}
	}
	else {
		for (int i = 0; i < pieceMap.size(); i++) {
			bitboard current = *(blackBitboards[i]);
			while (current) {
				bitboard lsb = Storage::getLSB(current);
				int sqIndex = Storage::bitboardToSqIndex(lsb);
				int row = sqIndex / 8;
				int col = sqIndex % 8;

				bitboard moveboard = Storage::getMoveboard(row, col, allWhite, allBlack, pieceMap[i], false);

				while (moveboard) {
					bitboard mlsb = Storage::getLSB(moveboard);
					Piece capture = Piece::None;

					if ((mlsb & allWhite) != 0) {
						for (int i = 0; i < pieceMap.size(); i++) {
							if ((*(whiteBitboards[i]) & mlsb) != 0) {
								capture = pieceMap[i];
								break;
							}
						}
					}

					Move newMove = { lsb, mlsb, pieceMap[i], false, (mlsb & allWhite) != 0, capture };
					
					// check if move puts king in check
					//makeMove(newMove);
					//if (!kingInCheck(false)) {
					moves.push_back(newMove);
					//}
					//undoMove(newMove);

					moveboard ^= mlsb;
				}

				current ^= lsb;
			}
		}
	}
	std::sort(moves.begin(), moves.end(), compareMoves);
	return moves;
}

bool Board::kingInCheck(bool white) {
	return (white ? wKing & attackingB : bKing & attackingW) != 0;
}

void Board::initZobrist() {
	std::random_device rd;
	std::mt19937_64 eng(rd());
	std::uniform_int_distribution<uint64_t> distr;

	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 64; j++) {
			zobristTable[i][j] = distr(eng);
		}
	}

	for (int i = 0; i < 16; i++) {
		zobristCastle[i] = distr(eng);
	}

	for (int file = 0; file < 8; file++) {
		zobristEnPassant[file] = distr(eng);
	}

	zobristSideToMove = distr(eng);
}

uint64_t Board::setZobristKey() {
	uint64_t key = 0;

	for (int i = 0; i < whiteBitboards.size(); i++) {
		bitboard current = *whiteBitboards[i];
		while (current) {
			bitboard lsb = Storage::getLSB(current);
			int sqIndex = Storage::bitboardToSqIndex(lsb);
			key ^= zobristTable[i][sqIndex];
			current ^= lsb;
		}
		current = *blackBitboards[i];
		while (current) {
			bitboard lsb = Storage::getLSB(current);
			int sqIndex = Storage::bitboardToSqIndex(lsb);
			key ^= zobristTable[i + 6][sqIndex];
			current ^= lsb;
		}
	}

	if (!getPlayer()) {
		key ^= zobristSideToMove;
	}

	zobristHash = key;

	return key;
}

uint64_t Board::getZobristKey() {
	return zobristHash;
}