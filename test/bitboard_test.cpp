#include "../src/bitboard.cpp"
#include <gtest/gtest.h>


TEST(BitboardTest, test_test_bit) { 
    Bitboard board = 1ULL;
    ASSERT_EQ(true, test_bit(a1, board));
    ASSERT_EQ(false, test_bit(b1, board));
}
 
TEST(BitboardTest, test_set_bit) {
    Bitboard board = 1ULL;
    ASSERT_EQ(1ULL, set_bit(a1, board));
    ASSERT_EQ(3ULL, set_bit(b1, board));
}

TEST(BitboardTest, test_toggle_bit) {
    Bitboard board = 1ULL;
    ASSERT_EQ(0ULL, toggle_bit(a1, board));
    ASSERT_EQ(3ULL, toggle_bit(b1, board));
}

TEST(BitboardTest, test_reset_bit) {
    Bitboard board = 1ULL;
    ASSERT_EQ(1ULL, reset_bit(b1, board));
    ASSERT_EQ(0ULL, reset_bit(a1, board));
}
