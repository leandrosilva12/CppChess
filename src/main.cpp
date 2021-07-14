
#include <iostream>
#include "bitboard.hpp"

int main() {
    // MagicNumbers::main();
    /* magic test q n funciona ou é demasiado fdd p eu entender
    Bitboards::print( 0x007E010101010100 );
    Bitboards::print( 0x48FFFE99FECFAA00 );	
    Bitboards::print( 0x01FE010101010101 );

    Bitboards::print( 0x01FE010101010101 * 0x48FFFE99FECFAA00 );
    Bitboards::print( 0x01FE010101010101 * 0x48FFFE99FECFAA00 >> (64 - 10) );

    Bitboards::print( transform(0x007E010101010100, 0x48FFFE99FECFAA00, 10) );
    */

    Bitboards::init();

    Bitboards::print( pieces[white] );
    Bitboards::print( pieces[white] & pieces[knight] );

    Bitboard block = 0ULL;
    block = set_bit(a6, block);
    block = set_bit(c3, block);
    block = set_bit(d4, block);
    block = set_bit(e5, block);

    Bitboards::print( block );
    
    Bitboards::print( pawn_attacks(0) );
    Bitboards::print( pawn_pushes(0) );
    Bitboards::print( knight_attacks(0) );
    Bitboards::print( king_attacks(0) );
    Bitboards::print( rook_attacks(a1, block) );
    Bitboards::print( bishop_attacks(a1, block) );
    Bitboards::print( queen_attacks(d4, block) );
    return 0;
}