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

const std::unordered_map<Piece, int> PIECE_VALUES = {
    {Piece::Pawn, 100},
    {Piece::Bishop, 300},
    {Piece::Knight, 300},
    {Piece::Rook, 500},
    {Piece::Queen, 900},
    {Piece::King, 10000}
};

struct Move {
    bitboard from;
    bitboard to;
    Piece piece;
    bool white;

    bool isCapture;
    Piece capturedPiece;

    int getScore() {
        if (isCapture) {
            return PIECE_VALUES.at(capturedPiece) - (PIECE_VALUES.at(capturedPiece) / 10);
        }
        return 0;
    }
};

class Board;
class Storage;
