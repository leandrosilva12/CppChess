
#include "types.hpp"


enum {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8,
};

enum {
    white, black,
    pawn, knight, bishop, rook, queen, king
};

extern Bitboard pieces[8];

namespace Bitboards {

    void init();
    void print(Bitboard);
}

bool test_bit(int square, Bitboard b);
Bitboard set_bit(int square, Bitboard b);
void toggle_bit_ref(int square, Bitboard *b);
Bitboard toggle_bit(int square, Bitboard b);
Bitboard reset_bit(int square, Bitboard b);

Bitboard pawn_attacks(int);
Bitboard pawn_pushes(int);
Bitboard pawn_en_passant(int, Bitboard);
Bitboard knight_attacks(int);
Bitboard king_attacks(int);

Bitboard rook_attacks(int, Bitboard);
Bitboard bishop_attacks(int, Bitboard);
Bitboard queen_attacks(int, Bitboard);

Bitboard bishop_attacks_on_the_fly(int, Bitboard);
Bitboard rook_attacks_on_the_fly(int, Bitboard);

Bitboard rook_mask(int);
Bitboard bishop_mask(int);