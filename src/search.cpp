

#include "search.hpp"
#include "bitboard.hpp"
#include <math.h>       /* log2 */

bool is_check(int color, Bitboard pieces[]) {
    Bitboard king_piece = pieces[king] & pieces[color];
    int king_square = log2(king_piece);
    Bitboard block = pieces[~color];
    
    Bitboard opPawns, opKnights, opRQ, opBQ;
    opPawns     = pieces[pawn] & pieces[~color];
    opKnights   = pieces[knight] & pieces[~color];
    opRQ = opBQ = pieces[queen] & pieces[~color];
    opRQ       |= pieces[rook] & pieces[~color];
    opBQ       |= pieces[bishop] & pieces[~color];
    return (pawn_attacks(~color) & king_piece)
        | (knight_attacks(~color) & king_piece)
        | (bishop_attacks (king_square, block)  & opBQ)
        | (rook_attacks   (king_square, block)  & opRQ)
        ;
}


//  calcular so legal moves  https://www.chessprogramming.org/Square_Attacked_By#LegalityTest
// legality test:
// Not in Check
//      The moving piece is not absolutely pinned on its move direction
//      En passant requires special horizontal pin test of both involved pawns, which disappear from the same rank
//      While castling, rook is not pinned horizontally in Chess960
// King in Check  ISTO_É_SO_PA_DIMINUR_A_PROCURA_DE_MOVIMENTOS_QND_EM_CHECK
//      Double Check
//          Only king moves to non attacked squares, sliding check x-rays the king
//      Single Check
//          Capture of checking piece. The capturing piece is not absolutely pinned
//          King moves to non attacked squares, sliding check x-rays the king
//          Interposing moves in case of distant sliding check. The moving piece is not absolutely pinned.


// "Two bits per side are appropriate to store the castling rights, often one uses one nibble to store the rights for both sides inside a position object, a kind a array of four bool"

Bitboard legal_moves(int color, Bitboard pieces[]) {

    // [(piece type, square from, square to),]


        
    // The moving piece is not absolutely pinned on its move direction
    



}



bool is_checkmate(int color, Bitboard pieces[]) {
    if(is_check(color, pieces))
        return;
}