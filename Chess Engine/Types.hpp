#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>

typedef uint64_t bitboard;
enum class Piece {
    Pawn,
    Bishop,
    Knight,
    Rook,
    King,
    Queen,
    None
};

const int PIECE_VALUES[7] = { 10, 30, 30, 50, 1000, 90, 0 };

struct Move {
    bitboard from;
    bitboard to;
    Piece piece;
    bool white;

    bool isCapture;
    Piece capturedPiece;

    int getScore() {
        if (isCapture) {
            return PIECE_VALUES[static_cast<int>(capturedPiece)] - (PIECE_VALUES[static_cast<int>(piece)] / 10);
        }
        return 0;
    }
};

class Board;
class Storage;
