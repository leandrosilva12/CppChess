
#include <bitset>
#include <iostream>
#include <algorithm>
#include "bitboard.h"

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

Bitboard r4_mask;
Bitboard r5_mask;
Bitboard cA_mask;
Bitboard cH_mask;
Bitboard cAB_mask;
Bitboard cGH_mask;

Bitboard pieces[8];
// pieces[8] = {
//   white_pieces,
//   black_pieces,
//   pawn_pieces,
//   knight_pieces,
//   bishop_pieces,
//   rook_pieces,
//   queen_pieces,
//   king_pieces,
// };
    

void Bitboards::init() {

    black_pieces = pieces[black] =  0xFFFF000000000000;
    white_pieces = pieces[white] =  0x000000000000FFFF;
    king_pieces = pieces[king] =   0x1000000000000010;
    queen_pieces = pieces[queen] =  0x0800000000000008;
    rook_pieces = pieces[rook] =   0x8100000000000081;
    bishop_pieces = pieces[bishop] = 0x2400000000000024;
    knight_pieces = pieces[knight] = 0x4200000000000042;
    pawn_pieces = pieces[pawn] =   0x00FF00000000FF00;

    black_en_passant = 0x0000000000000000;
    white_en_passant = 0x0000000000000000;

    r4_mask =  0x00000000FF000000;
    r5_mask =  0x000000FF00000000;
    cA_mask =  0xFEFEFEFEFEFEFEFE;
    cH_mask =  0x7F7F7F7F7F7F7F7F;
    cAB_mask = 0xfcfcfcfcfcfcfcfc;
    cGH_mask = 0x3f3f3f3f3f3f3f3f;

}


void Bitboards::print(Bitboard board) {
    std::bitset<64> bitboard = board;

    for(int i = 7; i >= 0; i--) {
        std::cout << i+1 << "  ";
        for(int j = 0; j < 8; j++) {
            std::cout << (bitboard[i*8 + j] ? "1 " : ". ");  
        }
        std::cout << std::endl;
    }    
    std::cout << "\n   A B C D E F G H\n\n";
}

bool test_bit(int square, Bitboard b) {return b & (1ULL << square);}
Bitboard set_bit(int square, Bitboard b) {return b | (1ULL << square);}
Bitboard toggle_bit(int square, Bitboard b) {return b ^ (1ULL << square);}
Bitboard reset_bit(int square, Bitboard b) {return b & ~(1ULL << square);}

Bitboard nort_one(Bitboard b) {return (b << 8);}
Bitboard sout_one(Bitboard b) {return (b >> 8);}
Bitboard west_one(Bitboard b) {return (b << 1) & cA_mask;}
Bitboard east_one(Bitboard b) {return (b >> 1) & cH_mask;}
Bitboard nw_one(Bitboard b) {return (b << 9) & cA_mask;}
Bitboard sw_one(Bitboard b) {return (b >> 7) & cA_mask;}
Bitboard se_one(Bitboard b) {return (b >> 9) & cH_mask;}
Bitboard ne_one(Bitboard b) {return (b << 7) & cH_mask;}

Bitboard pawn_attacks(int color) {

    Bitboard pawns = pawn_pieces & (color ? black_pieces : white_pieces);

    Bitboard east_attacks = color ? se_one(pawns) : ne_one(pawns);
    Bitboard west_attacks = color ? sw_one(pawns) : nw_one(pawns);

    // Prevents some impossible moves due to shifting
    Bitboard attacks = east_attacks | west_attacks;

    return (attacks & (color ? white_pieces : black_pieces)) | pawn_en_passant(color, attacks);
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

Bitboard pawn_en_passant(int color, Bitboard pawn_attacks) {

    // Bitboard pawns = pawn_pieces & (color ? black_pieces : white_pieces);
    Bitboard en_passant = color ? black_en_passant : white_en_passant;

    return (color ? nort_one(en_passant) : sout_one(en_passant)) & pawn_attacks;
}

Bitboard pawn_pushes(int color) {

    Bitboard single_pushes = pawn_single_push(color);
    Bitboard double_pushes = pawn_double_push(color);

    return single_pushes | (double_pushes & (color ? single_pushes >> 8 : single_pushes << 8));
}

Bitboard knight_attacks(int color) {

    Bitboard knights = knight_pieces & (color ? black_pieces : white_pieces);

    Bitboard l1 = (knights >> 1) & cH_mask;
    Bitboard l2 = (knights >> 2) & cGH_mask;
    Bitboard r1 = (knights << 1) & cA_mask;
    Bitboard r2 = (knights << 2) & cAB_mask;
    Bitboard h1 = l1 | r1;
    Bitboard h2 = l2 | r2;

    return ((h1<<16) | (h1>>16) | (h2<<8) | (h2>>8)) & ~(color ? black_pieces : white_pieces);
}

Bitboard king_attacks(int color) {
    Bitboard kings = king_pieces & (color ? black_pieces : white_pieces);
    Bitboard attacks = east_one(kings) | west_one(kings);
    kings |= attacks;
    attacks |= nort_one(kings) | sout_one(kings);

    return attacks & (color ? white_pieces : black_pieces);
}

Bitboard rook_attacks(int square, Bitboard block) {
  Bitboard attacks = 0ULL;
  int row = square/8, col = square%8, r, c;

  for(r = row+1; r <= 7; r++) {
    attacks = set_bit(col + r*8, attacks);
    if(test_bit(col + r*8, block)) break;
  }
  for(r = row-1; r >= 0; r--) {
    attacks = set_bit(col + r*8, attacks);
    if(test_bit(col + r*8, block)) break;
  }
  for(c = col+1; c <= 7; c++) {
    attacks = set_bit(c + row*8, attacks);
    if(test_bit(c + row*8, block)) break;
  }
  for(c = col-1; c >= 0; c--) {
    attacks = set_bit(c + row*8, attacks);
    if(test_bit(c + row*8, block)) break;
  }
  return attacks;
}

Bitboard bishop_attacks(int square, Bitboard block) {
  Bitboard attacks = 0ULL;
  int row = square/8, col = square%8, r, c;

  for(r = row+1, c = col+1; r <= 7 && c <= 7; r++, c++) {
    attacks = set_bit(c + r*8, attacks);
    if(test_bit(c + r*8, block)) break;
  }
  for(r = row+1, c = col-1; r <= 7 && c >= 0; r++, c--) {
    attacks = set_bit(c + r*8, attacks);
    if(test_bit(c + r*8, block)) break;
  }
  for(r = row-1, c = col+1; r >= 0 && c <= 7; r--, c++) {
    attacks = set_bit(c + r*8, attacks);
    if(test_bit(c + r*8, block)) break;
  }
  for(r = row-1, c = col-1; r >= 0 && c >= 0; r--, c--) {
    attacks = set_bit(c + r*8, attacks);
    if(test_bit(c + r*8, block)) break;
  }
  return attacks;
}


Bitboard queen_attacks(int square, Bitboard block) {
  return rook_attacks(square, block) | bishop_attacks(square, block);
}