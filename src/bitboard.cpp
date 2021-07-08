
#include <bitset>
#include <iostream>
#include "bitboard.h"

using namespace Bitboards;

Bitboard black_pieces;
Bitboard white_pieces;
Bitboard king_pieces;
Bitboard queen_pieces;
Bitboard rook_pieces;
Bitboard bishop_pieces; 
Bitboard knight_pieces;
Bitboard pawn_pieces; 
    

void Bitboards::init() {

    black_pieces =  0xFFFF000000000000;
    white_pieces =  0x000000000000FFFF;
    king_pieces =   0x0800000000000008;
    queen_pieces =  0x1000000000000010;
    rook_pieces =   0x8100000000000081;
    bishop_pieces = 0x2400000000000024;
    knight_pieces = 0x4200000000000042;
    pawn_pieces =   0x00FF00000000FF00;

}


void Bitboards::print(Bitboard board) {
    std::bitset<64> bitboard = board;

    for(int i=7; i>=0; i--) {
        for(int j=7; j>=0; j--) {
            std::cout << bitboard[i * 8 + j];  
        }
        std::cout << std::endl;
    }    
    std::cout << std::endl;

}


Bitboard pawn_attacks(int color) {

    Bitboard pawns = pawn_pieces & (color ? black_pieces : white_pieces);

    Bitboard pawn_east_atk = color ? pawns >> 9 : pawns << 7;
    Bitboard pawn_west_atk = color ? pawns >> 7 : pawns << 9;

    // Prevents some impossible moves due to shifting
    Bitboard a_col_mask = 0x7F7F7F7F7F7F7F7F;
    Bitboard h_col_mask = 0xFEFEFEFEFEFEFEFE;

    Bitboard pawn_attacks = ((pawn_east_atk & a_col_mask) | (pawn_west_atk & h_col_mask))
                            & (color ? white_pieces : black_pieces);

    Bitboards::print(pawn_attacks);
}

Bitboard pawn_single_push(int color) {

    Bitboard pawns = pawn_pieces & (color ? black_pieces : white_pieces);
    Bitboard empty = ~(black_pieces | white_pieces);
  
    Bitboard pawn_pushes = (color ? pawns >> 8 : pawns << 8) & empty;

    return pawn_pushes;
}

Bitboard pawn_double_push(int color) {

    Bitboard pawns = pawn_pieces & (color ? black_pieces : white_pieces);
    Bitboard empty = ~(black_pieces | white_pieces);

    Bitboard init_pawn_mask = color ? 0x000000FF00000000 : 0x00000000FF000000;
    Bitboard pawn_pushes = (color ? pawns >> 16 : pawns << 16) & empty & init_pawn_mask;

    return pawn_pushes;
}

Bitboard pawn_pushes(int color) {

    Bitboard single_pushes = pawn_single_push(color);
    Bitboard double_pushes = pawn_double_push(color);

    return single_pushes | (double_pushes & (color ? single_pushes >> 8 : single_pushes << 8));
}