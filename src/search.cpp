

#include "search.hpp"
#include "bitboard.hpp"
#include <math.h>       /* log2 */

Bitboard legal_moves(int color, Bitboard pieces[]) {
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
    return 0;
}
