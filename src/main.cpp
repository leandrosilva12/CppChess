
#include <iostream>
#include "bitboard.h"

int main() {
    Bitboards::init();

    pawn_attacks(1);
    pawn_pushes(0);
    return 0;
}