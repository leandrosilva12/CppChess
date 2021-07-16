
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

enum piece {
    white, black,
    pawn, knight, bishop, rook, queen, king
};

extern Bitboard pieces[8];

namespace Bitboards {

    void init();
    void print(Bitboard);
    void init_all();
}

bool test_bit(int, Bitboard);
Bitboard set_bit(int, Bitboard);
Bitboard toggle_bit(int, Bitboard);
Bitboard reset_bit(int, Bitboard);

Bitboard pawn_pushes(int);
Bitboard pawn_en_passant(int, Bitboard);

Bitboard pawn_attacks(int, int);
Bitboard knight_attacks(int);
Bitboard king_attacks(int);
Bitboard bishop_attacks(int, Bitboard);
Bitboard rook_attacks(int, Bitboard);
Bitboard queen_attacks(int, Bitboard);

Bitboard bishop_attacks_on_the_fly(int, Bitboard);
Bitboard rook_attacks_on_the_fly(int, Bitboard);

Bitboard bishop_mask(int);
Bitboard rook_mask(int);

Bitboard get_random_bitboard();
Bitboard generate_magic_number();
int count_bits();
int pop_ls1b(Bitboard*);