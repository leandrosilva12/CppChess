
#include <iostream>
#include "bitboard.h"

int main() {
    Bitboards::init();

    Bitboards::print( pawn_attacks(0) );
    Bitboards::print( pawn_pushes(0) );
    Bitboards::print( knight_attacks(0) );
    Bitboards::print( king_attacks(0) );
    return 0;
}