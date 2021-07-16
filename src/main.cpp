
#include <iostream>

#include "bitboard.hpp"
#include "magic_numbers.hpp"

int main() {
  // MagicNumbers::init();

  Bitboards::init();
  Bitboards::init_all();

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

  return 0;

  Bitboard block = 0ULL;
  block = set_bit(a6, block);
  block = set_bit(c3, block);
  block = set_bit(d4, block);
  block = set_bit(e5, block);

  Bitboards::print(block);
  std::cout << pop_ls1b(&block) << std::endl;
  Bitboards::print(block);

  Bitboards::print(get_random_bitboard());
  Bitboards::print(generate_magic_number());

  Bitboards::print(pawn_attacks(white, 0));
  Bitboards::print(pawn_pushes(0));
  Bitboards::print(knight_attacks(0));
  Bitboards::print(king_attacks(0));
  Bitboards::print(rook_attacks_on_the_fly(a1, block));
  Bitboards::print(bishop_attacks_on_the_fly(a1, block));
  Bitboards::print(rook_attacks_on_the_fly(d4, block));
  Bitboards::print(rook_mask(a1));
  Bitboards::print(bishop_mask(a1));
  Bitboards::print(rook_mask(d4));
  return 0;
}
