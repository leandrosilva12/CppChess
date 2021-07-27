

#include "search.hpp"
#include <assert.h>

#include <math.h>       /* log2 */

int check_type(int square, Bitboard pieces[]) {
    // test bit
    if (pieces[pawn] & (1ULL << square)) {
        return pawn;
    } else if (pieces[knight] & (1ULL << square)) {
        return knight;
    } else if (pieces[bishop] & (1ULL << square)) {
        return bishop;
    } else if (pieces[rook] & (1ULL << square)) {
        return rook;
    } else if (pieces[queen] & (1ULL << square)) {
        return queen;
    } else if (pieces[king] & (1ULL << square)){
        return king;
    }
    assert(false);
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

    Bitboard occupied = pieces[color];
    int square = a1;

    while (occupied) {
        if (occupied & 1) {
            int type = check_type(square, pieces);

            if (type == pawn) {

            }//eely a blocker or make a direct check.
        }
        square++;
        occupied <<= 1;
    }


        
    // The moving piece is not absolutely pinned on its move direction
    


    return 0;
}


Move* generate_pawn_moves() {
    return NULL;
}


Move* generate_moves(int type, int color, Bitboard pieces[], Move* moves) {
    assert(type != pawn && type != king);
    
    if(type == knight) {

    } else if(type == rook) {

    } else if(type == bishop) {

    } else if(type == queen) {

    }

    return moves;
}

