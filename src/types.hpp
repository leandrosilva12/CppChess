
#include <cstdint> // <stdint.h>

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

typedef uint64_t Bitboard;


enum Square: int {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8,
};

enum Color: int {
    white, black
};

enum Piece: int {
    white, black,
    pawn, knight, bishop, rook, queen, king
};



/// A move needs 16 bits to be stored
///
/// bit  0- 5: destination square (from 0 to 63)
/// bit  6-11: origin square (from 0 to 63)
/// bit 12-13: promotion piece type: knight (1) bishop (2) rook (3) queen (4)
/// bit 14-15: special move flag: promotion (1), en passant (2), castling (3)

typedef struct {

    int16_t move;
    int value;

} Move;


constexpr int Move_from(Move m) {
    return (m.move & 0xFC0) >> 6;
}

constexpr int Move_to(Move m) {
    return m.move & 0x3F;
}

constexpr int Move_promotion_type(Move m) {
    return (m.move & 0x2000) >> 12;
}

constexpr int Move_flag(Move m) {
    return (m.move & 0xC000) >> 14;
}

#endif
