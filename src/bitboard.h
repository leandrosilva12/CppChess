
#include "types.h"


namespace Bitboards {

    void init();
    void print(Bitboard);
}

Bitboard pawn_attacks(int);
Bitboard pawn_pushes(int);
