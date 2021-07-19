
#include <iostream>

#include "bitboard.hpp"

int main() {
  // MagicNumbers::init();

  Bitboards::init();

  // define test bitboard
  Bitboard occupancy = 0ULL;

  // set blocker pieces on board
  occupancy = set_bit(c5, occupancy);
  occupancy = set_bit(f2, occupancy);
  occupancy = set_bit(g7, occupancy);
  occupancy = set_bit(b2, occupancy);
  occupancy = set_bit(g5, occupancy);
  occupancy = set_bit(e2, occupancy);
  occupancy = set_bit(e7, occupancy);
  occupancy = set_bit(e4, occupancy);
  occupancy = set_bit(e3, occupancy);

  // print occupancies
  Bitboards::print(occupancy);

  // print rook attacks
  Bitboards::print(rook_attacks(e5, occupancy));

  // print bishop attacks
  Bitboards::print(bishop_attacks(d4, occupancy));

  Bitboards::print(pawn_attacks(white, d4));
  Bitboards::print(knight_attacks(d4));
  Bitboards::print(king_attacks(d4));
  return 0;
}
