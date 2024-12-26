#include "Storage.hpp"
#include "Board.hpp"

#include <iostream>

bitboard Storage::pawnMaskWhiteTake[8][8];
bitboard Storage::pawnMaskBlackTake[8][8];

bitboard Storage::pawnMaskWhite[8][8];
bitboard Storage::pawnMaskBlack[8][8];

bitboard Storage::knightMask[8][8];
bitboard Storage::kingMask[8][8];

bitboard Storage::rookBlockers[8][8];
bitboard Storage::bishopBlockers[8][8];

int Storage::rookShifts[8][8];
int Storage::bishopShifts[8][8];

vector<vector<bitboard>> Storage::rookDB;
vector<vector<bitboard>> Storage::bishopDB;

void Storage::init() {
	rookDB.resize(64, vector<bitboard>(1ULL << 12, 0));
	bishopDB.resize(64, vector<bitboard>(1ULL << 9, 0));

	std::cout << "Initializing database...\n";
	generateMoves();
	generateShifts();
	initDB();
	std::cout << "Database initialization finished.\n";
}

void Storage::generateMoves() {
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			pawnMaskWhiteTake[row][col] = getPawnMovesTake(row, col, true);
			pawnMaskBlackTake[row][col] = getPawnMovesTake(row, col, false);

			pawnMaskWhite[row][col] = getPawnMoves(row, col, true);
			pawnMaskBlack[row][col] = getPawnMoves(row, col, false);

			rookBlockers[row][col] = getRookMoves(row, col);
			knightMask[row][col] = getKnightMoves(row, col);
			bishopBlockers[row][col] = getBishopMoves(row, col);
			kingMask[row][col] = getKingMoves(row, col);
		}
	}
}

bitboard Storage::getPawnMovesTake(int row, int col, bool white) {
	bitboard result = 0;
	if (white ? row > 0 : row < 7) {
		if (col > 0) {
			result |= Board::getBitboard(row + (white ? -1 : 1), col - 1);
		}
		if (col < 7) {
			result |= Board::getBitboard(row + (white ? -1 : 1), col + 1);
		}
	}
	return result;
}

bitboard Storage::getPawnMoves(int row, int col, bool white) {
	bitboard result = 0;
	if (white ? row > 0 : row < 7) {
		result |= Board::getBitboard(row + (white ? -1 : 1), col);
		if (white && row == 6) {
			result |= Board::getBitboard(row - 2, col);
		}
		else if (!white && row == 1) {
			result |= Board::getBitboard(row + 2, col);
		}
	}
	return result;
}

bitboard Storage::getRookMoves(int row, int col) {
	bitboard result = 0;
	for (int r = 0; r < 8; r++) {
		result |= Board::getBitboard(r, col);
	}
	for (int c = 0; c < 8; c++) {
		result |= Board::getBitboard(row, c);
	}

	result &= ~(Board::getBitboard(row, col));
	result &= ~(Board::getBitboard(row, 0));
	result &= ~(Board::getBitboard(row, 7));
	result &= ~(Board::getBitboard(0, col));
	result &= ~(Board::getBitboard(7, col));

	return result;
}

bitboard Storage::getKnightMoves(int row, int col) {
	bitboard result = 0;
	int coordDeltas[][2] = {{-2, 1}, {-2, -1}, {2, 1}, {2, -1}, {-1, 2}, {-1, -2}, {1, 2}, {1, -2}};
	
	for (int i = 0; i < 8; i++) {
		int r = row + coordDeltas[i][0], c = col + coordDeltas[i][1];
		if (r >= 0 && r < 8 && c >= 0 && c < 8) {
			result |= Board::getBitboard(r, c);
		}
	}

	return result;
}

bitboard Storage::getBishopMoves(int row, int col) {
	bitboard result = 0;
	for (int r = 0; r < 8; r++) {
		for (int c = 0; c < 8; c++) {
			if (abs(row - r) == abs(col - c) && r != row) {
				result |= Board::getBitboard(r, c);
			}
		}
	}

	return result & (~EDGES);
}

bitboard Storage::getKingMoves(int row, int col) {
	bitboard result = 0;
	for (int r = row - 1; r < row + 2; r++) {
		for (int c = col - 1; c < col + 2; c++) {
			if (r == row && c == col) continue;
			if (r >= 0 && r < 8 && c >= 0 && c < 8) {
				result |= Board::getBitboard(r, c);
			}
		}
	}
	return result;
}

int Storage::popcount(bitboard b) {
	int c = 0;
	while (b) {
		b &= b - 1;
		c++;
	}
	return c;
}

void Storage::generateShifts() {
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			rookShifts[row][col] = 64 - popcount(rookBlockers[row][col]);
			bishopShifts[row][col] = 64 - popcount(bishopBlockers[row][col]);
		}
	}
}

uint64_t Storage::getIndex(int row, int col, Piece piece, bitboard blockerBoard) {
	return (blockerBoard * (piece == Piece::Rook ? rookMagics[row][col] : bishopMagics[row][col])) >> (piece == Piece::Rook ? rookShifts[row][col] : bishopShifts[row][col]);
}

bitboard Storage::generateBlockerboard(uint64_t index, bitboard blockermask) {
	bitboard blockerboard = blockermask;
	int bitIndex = 0;

	for (int i = 0; i < 64; i++) {
		if ((blockermask & Board::getBitboard(63 - i)) != 0) {
			if ((index & Board::getBitboard(63 - bitIndex)) == 0) {
				blockerboard &= ~(Board::getBitboard(63 - i));
			}
			bitIndex++;
		}
	}
	return blockerboard;
}

bitboard Storage::getMoveboard(int row, int col, bitboard enemy, bitboard friendly, Piece piece, bool white) {
	bitboard occ = enemy | friendly;
	
	switch (piece) {
		case Piece::Pawn:
			if (white) {
				bitboard normalMove = pawnMaskWhite[row][col] & ~occ;

				if (row == 6) {
					normalMove &= ~((occ & Board::getBitboard(row - 1, col)) << 8);
				}

				bitboard take = pawnMaskWhiteTake[row][col] & enemy;

				return take | normalMove;
			}
			else {
				bitboard normalMove = pawnMaskBlack[row][col] & ~occ;

				if (row == 1) {
					normalMove &= ~((occ & Board::getBitboard(row + 1, col)) >> 8);
				}

				bitboard take = pawnMaskBlackTake[row][col] & enemy;

				return take | normalMove;
			}
		case Piece::Knight:
			return knightMask[row][col] & ~friendly;
		case Piece::King:
			return kingMask[row][col] & ~friendly;
		case Piece::Rook:
			return rookDB[Board::getSquareIndex(row, col)][getIndex(row, col, piece, rookBlockers[row][col] & occ)] & ~friendly;
		case Piece::Bishop:
			return bishopDB[Board::getSquareIndex(row, col)][getIndex(row, col, piece, bishopBlockers[row][col] & occ)] & ~friendly;
		case Piece::Queen:
			return getMoveboard(row, col, enemy, friendly, Piece::Rook, white) | getMoveboard(row, col, enemy, friendly, Piece::Bishop, white);
		default:
			return 0;
	}
}

bitboard Storage::getAttackingMoveboard(int row, int col, bitboard enemy, bitboard friendly, Piece piece, bool white) {
	bitboard occ = enemy | friendly;
	
	switch (piece) {
		case Piece::Pawn:
			if (white) {
				return pawnMaskWhiteTake[row][col];
			}
			else {
				return pawnMaskBlackTake[row][col];
			}
		case Piece::Knight:
			return knightMask[row][col];
		case Piece::King:
			return kingMask[row][col];
		case Piece::Rook:
			return rookDB[Board::getSquareIndex(row, col)][getIndex(row, col, piece, rookBlockers[row][col] & occ)];
		case Piece::Bishop:
			return bishopDB[Board::getSquareIndex(row, col)][getIndex(row, col, piece, bishopBlockers[row][col] & occ)];
		case Piece::Queen:
			return getAttackingMoveboard(row, col, enemy, friendly, Piece::Rook, white) | getAttackingMoveboard(row, col, enemy, friendly, Piece::Bishop, white);
		default:
			return 0;
	}
}

bitboard Storage::generateRookMoveboard(int row, int col, bitboard blockerboard) {
	bitboard moveboard = 0;

	// up moves
	int r = row;
	while (r > 0 && !Board::oneAtBitboard(r, col, blockerboard)) {
		r--;
		moveboard |= Board::getBitboard(r, col);
	}

	// down moves
	r = row;
	while (r < 7 && !Board::oneAtBitboard(r, col, blockerboard)) {
		r++;
		moveboard |= Board::getBitboard(r, col);
	}

	// left moves
	int c = col;
	while (c > 0 && !Board::oneAtBitboard(row, c, blockerboard)) {
		c--;
		moveboard |= Board::getBitboard(row, c);
	}

	// right moves
	c = col;
	while (c < 7 && !Board::oneAtBitboard(row, c, blockerboard)) {
		c++;
		moveboard |= Board::getBitboard(row, c);
	}

	return moveboard;
}

bitboard Storage::generateBishopMoveboard(int row, int col, bitboard blockerboard) {
	bitboard moveboard = 0;

	// up left
	int r = row;
	int c = col;
	while (r > 0 && c > 0 && !Board::oneAtBitboard(r, c, blockerboard)) {
		r--;
		c--;
		moveboard |= Board::getBitboard(r, c);
	}

	// up right
	r = row;
	c = col;
	while (r > 0 && c < 7 && !Board::oneAtBitboard(r, c, blockerboard)) {
		r--;
		c++;
		moveboard |= Board::getBitboard(r, c);
	}

	// down left
	r = row;
	c = col;
	while (r < 7 && c > 0 && !Board::oneAtBitboard(r, c, blockerboard)) {
		r++;
		c--;
		moveboard |= Board::getBitboard(r, c);
	}
	
	// down right
	r = row;
	c = col;
	while (r < 7 && c < 7 && !Board::oneAtBitboard(r, c, blockerboard)) {
		r++;
		c++;
		moveboard |= Board::getBitboard(r, c);
	}

	return moveboard;
}

void Storage::initDB() {
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			bitboard rMask = rookBlockers[row][col];

			for (uint64_t i = 0; i < (1ULL << popcount(rMask)); i++) {
				bitboard blockerboard = generateBlockerboard(i, rMask);
				bitboard moveboard = generateRookMoveboard(row, col, blockerboard);
				uint64_t index = getIndex(row, col, Piece::Rook, blockerboard);
				rookDB[Board::getSquareIndex(row, col)][index] = moveboard;
			}

			bitboard bMask = bishopBlockers[row][col];
			
			for (uint64_t i = 0; i < (1ULL << popcount(bMask)); i++) {
				bitboard blockerboard = generateBlockerboard(i, bMask);
				bitboard moveboard = generateBishopMoveboard(row, col, blockerboard);
				uint64_t index = getIndex(row, col, Piece::Bishop, blockerboard);
				bishopDB[Board::getSquareIndex(row, col)][index] = moveboard;
			}
		}
	}
}

const uint64_t Storage::rookMagics[8][8] = {
	{
		562927068959138, 281477157749761, 281483633756161, 281483634278417,
		4432674693377, 18014183758528662, 36025352403644746, 72054149422608714,
	}, {
		140738578874496, 140741783453824, 2199090397312, 4398180761728,
		8796361490560, 17592722948224, 35185445863552, 35459258384512,
	}, {
		277042298884, 1099545215104, 2199090397312, 4398180761728,
		8796361490560, 17592722948224, 35185445863552, 35459258417152,
	}, {
		140738570486016, 2199040098308, 2201179128832, 4400202385408,
		8798248898560, 17594341924864, 35185450029056, 35459258384512,
	}, {
		140739635855616, 1101667500544, 2201171001472, 4400194519168,
		8798241554560, 17594335625344, 35185445851136, 35736275402752,
	}, {
		2199027482884, 1103806726148, 141287277724672, 141287311280128,
		141287378391040, 141287512612864, 70643624185856, 35734132097152,
	}, {
		140738570486016, 140741783453824, 140746078552192, 140754668748928,
		140771849142400, 140806209929344, 70369281069056, 140738029420672,
	}, {
		36028935540048128, 36029346791555584, 36029905120788608, 36031013222613120,
		36033229426262144, 36037661833560192, 18014467231055936, 36028866279506048,
	}
};

const uint64_t Storage::bishopMagics[8][8] = {
	{
		565157600297472, 4415293752320, 17247502848, 268567040,
		2131968, 545525760, 139620524032, 17871427092480,
	}, {
		565157600296960, 1130315200593920, 4415360729088, 68753162240,
		545783808, 139654594560, 35742854152192, 71485708304384,
	}, {
		282578800083456, 565157600166912, 282578800083456, 8800392184832,
		137724168192, 8935813681152, 71485704118272, 142971408236544,
	}, {
		141289391719424, 282578783438848, 141291539267840, 70644696088832,
		2201171263616, 17884244346880, 142970872401920, 285941744803840,
	}, {
		70643630606336, 141287261212672, 70643655708672, 145135543263232,
		70643689259520, 35734195078144, 285873157965824, 571746315931648,
	}, {
		35185462874112, 70370925748224, 35188675985408, 140754678710272,
		140771881664512, 281483600331264, 562967200662528, 1125934401325056,
	}, {
		139620524032, 279241048064, 558618378240, 1169304846336,
		4415234768896, 4402375163904, 2207646876160, 4415293752320,
	}, {
		17871427092480, 71485708304384, 143006304829440, 299342040662016,
		1130300100837376, 1127008041959424, 565157600296960, 565157600297472,
	}
};

bitboard Storage::getLSB(bitboard b) {
	return b & (~b + 1);
}

int Storage::bitboardToSqIndex(bitboard b) {
	int i = 63;
	while (i > 0 && (b & 1) == 0) {
		b >>= 1;
		i--;
	}
	return i;
}