
#include "bitboard.hpp"

#include <algorithm>
#include <bitset>
#include <chrono>
#include <cstring>
#include <iostream>
using namespace std::chrono;

using namespace Bitboards;

Bitboard white_pieces;
Bitboard black_pieces;
Bitboard pawn_pieces;
Bitboard knight_pieces;
Bitboard bishop_pieces;
Bitboard rook_pieces;
Bitboard queen_pieces;
Bitboard king_pieces;

Bitboard black_en_passant;
Bitboard white_en_passant;

Bitboard pieces[8];

Bitboard rook_magic_numbers[64];
Bitboard bishop_magic_numbers[64];

Bitboard pawn_attacks_table[2][64];  // [color][square]
Bitboard knight_attacks_table[64];
Bitboard king_attacks_table[64];

Bitboard bishop_masks_table[64];
Bitboard rook_masks_table[64];

Bitboard bishop_attacks_table[64][512];  // [square][occupancies]
Bitboard rook_attacks_table[64][4096];   // [square][occupancies]

const Bitboard r4_mask =  0x00000000FF000000;
const Bitboard r5_mask =  0x000000FF00000000;
const Bitboard cA_mask =  0xFEFEFEFEFEFEFEFE;
const Bitboard cH_mask =  0x7F7F7F7F7F7F7F7F;
const Bitboard cAB_mask = 0xfcfcfcfcfcfcfcfc;
const Bitboard cGH_mask = 0x3f3f3f3f3f3f3f3f;

// results for hash of pop LS1B
const int bits_table[64] = {
  63, 30, 3,  32, 25, 41, 22, 33, 15, 50, 42, 13, 11,
  53, 19, 34, 61, 29, 2,  51, 21, 43, 45, 10, 18, 47,
  1,  54, 9,  57, 0,  35, 62, 31, 40, 4,  49, 5,  52,
  26, 60, 6,  23, 44, 46, 27, 56, 16, 7,  39, 48, 24,
  59, 14, 12, 55, 38, 28, 58, 20, 37, 17, 36, 8};

// bishop relevant occupancy bit count for every square on board
const int bishop_relevant_bits[64] = {
  6, 5, 5, 5, 5, 5, 5, 6,
  5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5,
  6, 5, 5, 5, 5, 5, 5, 6};

// rook relevant occupancy bit count for every square on board
const int rook_relevant_bits[64] = {
  12, 11, 11, 11, 11, 11, 11, 12,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  12, 11, 11, 11, 11, 11, 11, 12};


void Bitboards::print(Bitboard board) {
  std::bitset<64> bitboard = board;

  for (int i = 7; i >= 0; i--) {
    std::cout << i + 1 << "  ";
    for (int j = 0; j < 8; j++) {
      std::cout << (bitboard[i * 8 + j] ? "1 " : ". ");
    }
    std::cout << std::endl;
  }
  std::cout << "\n   A B C D E F G H\n\n";
}

bool test_bit(int square, Bitboard b) { return b & (1ULL << square); }
Bitboard set_bit(int square, Bitboard b) { return b | (1ULL << square); }
Bitboard toggle_bit(int square, Bitboard b) { return b ^ (1ULL << square); }
Bitboard reset_bit(int square, Bitboard b) { return b & ~(1ULL << square); }

Bitboard nort_one(Bitboard b) { return (b << 8); }
Bitboard sout_one(Bitboard b) { return (b >> 8); }
Bitboard west_one(Bitboard b) { return (b << 1) & cA_mask; }
Bitboard east_one(Bitboard b) { return (b >> 1) & cH_mask; }
Bitboard nw_one(Bitboard b) { return (b << 9) & cA_mask; }
Bitboard sw_one(Bitboard b) { return (b >> 7) & cA_mask; }
Bitboard se_one(Bitboard b) { return (b >> 9) & cH_mask; }
Bitboard ne_one(Bitboard b) { return (b << 7) & cH_mask; }

Bitboard all_pawn_attacks(int color) {
  Bitboard pawns = pawn_pieces & (color ? black_pieces : white_pieces);

  Bitboard east_attacks = color ? se_one(pawns) : ne_one(pawns);
  Bitboard west_attacks = color ? sw_one(pawns) : nw_one(pawns);

  // Prevents some impossible moves due to shifting
  Bitboard attacks = east_attacks | west_attacks;

  return (attacks & (color ? white_pieces : black_pieces)) |
         pawn_en_passant(color, attacks);
}

Bitboard pawn_single_push(int color) {
  Bitboard pawns = pawn_pieces & (color ? black_pieces : white_pieces);
  Bitboard empty = ~(black_pieces | white_pieces);

  return (color ? sout_one(pawns) : nort_one(pawns)) & empty;
}

Bitboard pawn_double_push(int color) {
  Bitboard pawns = pawn_pieces & (color ? black_pieces : white_pieces);
  Bitboard empty = ~(black_pieces | white_pieces);

  Bitboard double_push_mask = color ? r5_mask : r4_mask;

  return (color ? pawns >> 16 : pawns << 16) & empty & double_push_mask;
}

Bitboard pawn_pushes(int color) {
  Bitboard single_pushes = pawn_single_push(color);
  Bitboard double_pushes = pawn_double_push(color);

  return single_pushes |
         (double_pushes & (color ? single_pushes >> 8 : single_pushes << 8));
}

Bitboard pawn_en_passant(int color, Bitboard pawn_attacks) {
  // Bitboard pawns = pawn_pieces & (color ? black_pieces : white_pieces);
  Bitboard en_passant = color ? black_en_passant : white_en_passant;

  return (color ? nort_one(en_passant) : sout_one(en_passant)) & pawn_attacks;
}

Bitboard all_knight_attacks(int color) {
  Bitboard knights = knight_pieces & (color ? black_pieces : white_pieces);

  Bitboard l1 = (knights >> 1) & cH_mask;
  Bitboard l2 = (knights >> 2) & cGH_mask;
  Bitboard r1 = (knights << 1) & cA_mask;
  Bitboard r2 = (knights << 2) & cAB_mask;
  Bitboard h1 = l1 | r1;
  Bitboard h2 = l2 | r2;

  return ((h1 << 16) | (h1 >> 16) | (h2 << 8) | (h2 >> 8)) &
         ~(color ? black_pieces : white_pieces);
}

Bitboard all_king_attacks(int color) {
  Bitboard kings = king_pieces & (color ? black_pieces : white_pieces);
  Bitboard attacks = east_one(kings) | west_one(kings);
  kings |= attacks;
  attacks |= nort_one(kings) | sout_one(kings);

  return attacks & (color ? white_pieces : black_pieces);
}

Bitboard pawn_attacks(int color, int square) {
  Bitboard pawn = set_bit(square, 0ULL);

  Bitboard east_attacks = color ? se_one(pawn) : ne_one(pawn);
  Bitboard west_attacks = color ? sw_one(pawn) : nw_one(pawn);

  return east_attacks | west_attacks;
}

Bitboard knight_attacks(int square) {
  Bitboard knight = set_bit(square, 0ULL);

  Bitboard l1 = (knight >> 1) & cH_mask;
  Bitboard l2 = (knight >> 2) & cGH_mask;
  Bitboard r1 = (knight << 1) & cA_mask;
  Bitboard r2 = (knight << 2) & cAB_mask;
  Bitboard h1 = l1 | r1;
  Bitboard h2 = l2 | r2;

  return (h1 << 16) | (h1 >> 16) | (h2 << 8) | (h2 >> 8);
}

Bitboard king_attacks(int square) {
  Bitboard king = set_bit(square, 0ULL);
  Bitboard attacks = east_one(king) | west_one(king);
  king |= attacks;
  attacks |= nort_one(king) | sout_one(king);

  return attacks;
}

Bitboard rook_attacks_on_the_fly(int square, Bitboard block) {
  Bitboard attacks = 0ULL;
  int row = square / 8, col = square % 8, r, c;

  for (r = row + 1; r <= 7; r++) {
    attacks = set_bit(col + r * 8, attacks);
    if (test_bit(col + r * 8, block)) break;
  }
  for (r = row - 1; r >= 0; r--) {
    attacks = set_bit(col + r * 8, attacks);
    if (test_bit(col + r * 8, block)) break;
  }
  for (c = col + 1; c <= 7; c++) {
    attacks = set_bit(c + row * 8, attacks);
    if (test_bit(c + row * 8, block)) break;
  }
  for (c = col - 1; c >= 0; c--) {
    attacks = set_bit(c + row * 8, attacks);
    if (test_bit(c + row * 8, block)) break;
  }
  return attacks;
}

Bitboard bishop_attacks_on_the_fly(int square, Bitboard block) {
  Bitboard attacks = 0ULL;
  int row = square / 8, col = square % 8, r, c;

  for (r = row + 1, c = col + 1; r <= 7 && c <= 7; r++, c++) {
    attacks = set_bit(c + r * 8, attacks);
    if (test_bit(c + r * 8, block)) break;
  }
  for (r = row + 1, c = col - 1; r <= 7 && c >= 0; r++, c--) {
    attacks = set_bit(c + r * 8, attacks);
    if (test_bit(c + r * 8, block)) break;
  }
  for (r = row - 1, c = col + 1; r >= 0 && c <= 7; r--, c++) {
    attacks = set_bit(c + r * 8, attacks);
    if (test_bit(c + r * 8, block)) break;
  }
  for (r = row - 1, c = col - 1; r >= 0 && c >= 0; r--, c--) {
    attacks = set_bit(c + r * 8, attacks);
    if (test_bit(c + r * 8, block)) break;
  }
  return attacks;
}

Bitboard rook_mask(int square) {
  Bitboard mask = 0ULL;
  int row = square / 8, col = square % 8, r, c;

  for (r = row + 1; r <= 6; r++) mask = set_bit(col + r * 8, mask);
  for (r = row - 1; r >= 1; r--) mask = set_bit(col + r * 8, mask);
  for (c = col + 1; c <= 6; c++) mask = set_bit(c + row * 8, mask);
  for (c = col - 1; c >= 1; c--) mask = set_bit(c + row * 8, mask);

  return mask;
}

Bitboard bishop_mask(int square) {
  Bitboard mask = 0ULL;
  int row = square / 8, col = square % 8, r, c;

  for (r = row + 1, c = col + 1; r <= 6 && c <= 6; r++, c++)
    mask = set_bit(c + r * 8, mask);
  for (r = row + 1, c = col - 1; r <= 6 && c >= 1; r++, c--)
    mask = set_bit(c + r * 8, mask);
  for (r = row - 1, c = col + 1; r >= 1 && c <= 6; r--, c++)
    mask = set_bit(c + r * 8, mask);
  for (r = row - 1, c = col - 1; r >= 1 && c >= 1; r--, c--)
    mask = set_bit(c + r * 8, mask);

  return mask;
}

Bitboard get_random_bitboard() {
  Bitboard u1, u2, u3, u4;
  u1 = (Bitboard)(random()) & 0xFFFF;
  u2 = (Bitboard)(random()) & 0xFFFF;
  u3 = (Bitboard)(random()) & 0xFFFF;
  u4 = (Bitboard)(random()) & 0xFFFF;
  return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

Bitboard generate_magic_number() {
  return get_random_bitboard() & get_random_bitboard() & get_random_bitboard();
}

int count_bits(Bitboard b) {
  int r;
  for (r = 0; b; r++, b &= b - 1)
    ;
  return r;
}

int pop_ls1b(Bitboard *bb) {
  Bitboard b = *bb ^ (*bb - 1);
  unsigned int fold = (unsigned)((b & 0xffffffff) ^ (b >> 32));
  *bb &= (*bb - 1);
  return bits_table[(fold * 0x783a9b23) >> 26];
}

void init_leapers_attacks() {
  for (int sq = a1; sq <= h8; sq++) {
    // init pawn attacks
    pawn_attacks_table[white][sq] = pawn_attacks(white, sq);
    pawn_attacks_table[black][sq] = pawn_attacks(black, sq);

    // init knight attacks
    knight_attacks_table[sq] = knight_attacks(sq);

    // init king attacks
    king_attacks_table[sq] = king_attacks(sq);
  }
}

Bitboard get_occupancy(int index, int bits, Bitboard attack_mask) {
  int i, j;
  Bitboard occupancy = 0ULL;
  for (i = 0; i < bits; i++) {
    j = pop_ls1b(&attack_mask);
    if (index & (1 << i)) occupancy |= (1ULL << j);
  }
  return occupancy;
}

int get_magic_index(Bitboard b, Bitboard magic, int bits) {
  return (int)((b * magic) >> (64 - bits));
}

/** Faster than chessprogramming approach **/
Bitboard find_magic_number(int square, int relevant_bits, int bishop) {
  // init occupancies
  Bitboard occupancies[4096];

  // init attack tables
  Bitboard attacks[4096];

  // init used attacks
  Bitboard used_attacks[4096];

  // init attack mask for a current piece
  Bitboard attack_mask = bishop ? bishop_mask(square) : rook_mask(square);

  // init occupancy indicies
  int occupancy_indicies = 1 << relevant_bits;

  // loop over occupancy indicies
  for (int index = 0; index < occupancy_indicies; index++) {
    // init occupancies
    occupancies[index] = get_occupancy(index, relevant_bits, attack_mask);

    // init attacks
    attacks[index] = bishop
                         ? bishop_attacks_on_the_fly(square, occupancies[index])
                         : rook_attacks_on_the_fly(square, occupancies[index]);
  }

  // test magic numbers loop
  for (int random_count = 0; random_count < 100000000; random_count++) {
    // generate magic number candidate
    Bitboard magic_number = generate_magic_number();

    // skip inappropriate magic numbers
    if (count_bits((attack_mask * magic_number) & 0xFF00000000000000) < 6)
      continue;

    // init used attacks
    memset(used_attacks, 0ULL, sizeof(used_attacks));

    // init index & fail flag
    int index, fail;

    // test magic index loop
    for (index = 0, fail = 0; !fail && index < occupancy_indicies; index++) {
      // init magic index
      int magic_index =
          (int)((occupancies[index] * magic_number) >> (64 - relevant_bits));

      if (used_attacks[magic_index] == 0ULL)
        // magic index works
        // init used attacks
        used_attacks[magic_index] = attacks[index];  //

      else if (used_attacks[magic_index] != attacks[index])
        // magic index doesn't work
        fail = 1;
    }

    if (!fail) return magic_number;
  }

  std::cout << "***Failed***" << std::endl;
  return 0ULL;
}

Bitboard rook_attacks(int square, Bitboard occupancy) {
  // get rook attacks assuming current board occupancy
  int magic_index =
      get_magic_index(occupancy & rook_masks_table[square],
                      rook_magic_numbers[square], rook_relevant_bits[square]);

  return rook_attacks_table[square][magic_index];
}

Bitboard bishop_attacks(int square, Bitboard occupancy) {
  // get bishop attacks assuming current board occupancy
  int magic_index = get_magic_index(occupancy & bishop_masks_table[square],
                                    bishop_magic_numbers[square],
                                    bishop_relevant_bits[square]);

  return bishop_attacks_table[square][magic_index];
}

Bitboard queen_attacks(int square, Bitboard occupancy) {
  return rook_attacks(square, occupancy) | bishop_attacks(square, occupancy);
}

void init_magic_numbers() {
  for (int sq = a1; sq <= h8; sq++)
    // init rook magic numbers
    rook_magic_numbers[sq] =
        find_magic_number(sq, rook_relevant_bits[sq], 0);

  for (int sq = a1; sq <= h8; sq++)
    // init bishop magic numbers
    bishop_magic_numbers[sq] =
        find_magic_number(sq, bishop_relevant_bits[sq], 1);
}

void init_sliders_attacks(int bishop) {
  for (int sq = a1; sq <= h8; sq++) {
    Bitboard attack_mask;

    // init mask
    if (bishop) {
      bishop_masks_table[sq] = bishop_mask(sq);
      attack_mask = bishop_masks_table[sq];
    } else {
      rook_masks_table[sq] = rook_mask(sq);
      attack_mask = rook_masks_table[sq];
    }

    // init relevant occupancy bit count
    int relevant_bits_count = count_bits(attack_mask);

    // init occupancy indicies
    int occupancy_indicies = (1 << relevant_bits_count);

    // loop over occupancy indicies
    for (int index = 0; index < occupancy_indicies; index++) {
      if (bishop) {
        // init current occupancy variation
        Bitboard occupancy =
            get_occupancy(index, relevant_bits_count, attack_mask);

        // init magic index
        int magic_index = get_magic_index(occupancy, bishop_magic_numbers[sq],
                                          bishop_relevant_bits[sq]);

        // init bishop attacks
        bishop_attacks_table[sq][magic_index] =
            bishop_attacks_on_the_fly(sq, occupancy);
      } else {
        // init current occupancy variation
        Bitboard occupancy =
            get_occupancy(index, relevant_bits_count, attack_mask);

        // init magic index
        int magic_index = get_magic_index(occupancy, rook_magic_numbers[sq],
                                          rook_relevant_bits[sq]);

        // init bishop attacks
        rook_attacks_table[sq][magic_index] =
            rook_attacks_on_the_fly(sq, occupancy);
      }
    }
  }
}

void Bitboards::init() {
  black_pieces = pieces[black] = 0xFFFF000000000000;
  white_pieces = pieces[white] = 0x000000000000FFFF;
  king_pieces = pieces[king] = 0x1000000000000010;
  queen_pieces = pieces[queen] = 0x0800000000000008;
  rook_pieces = pieces[rook] = 0x8100000000000081;
  bishop_pieces = pieces[bishop] = 0x2400000000000024;
  knight_pieces = pieces[knight] = 0x4200000000000042;
  pawn_pieces = pieces[pawn] = 0x00FF00000000FF00;

  black_en_passant = 0x0000000000000000;
  white_en_passant = 0x0000000000000000;

  // init magic numbers
  init_magic_numbers();

  // init leaper pieces attacks
  init_leapers_attacks();

  // init slider pieces attacks
  init_sliders_attacks(0);
  init_sliders_attacks(1);
}
