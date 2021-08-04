#include "types.hpp"
#include "bitboard.hpp"

#ifndef POSITION_H_INCLUDED
#define POSITION_H_INCLUDED

class Position {
 public:
    Position& set();
    void do_move();
    void undo_move();

 private:
  Bitboard pieces[8];
};

#endif
