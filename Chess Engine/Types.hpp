#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>

typedef uint64_t bitboard;
enum class Piece {
    Pawn,
    Rook,
    Knight,
    Bishop,
    Queen,
    King,
    None
};

struct Move {
    bitboard from;
    bitboard to;
    Piece piece;
    bool white;

    bool isCapture;
    Piece capturedPiece;
};

class Board;
class Storage;