
#include "types.hpp"


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