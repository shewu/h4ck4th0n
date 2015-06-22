#include "gtest/gtest.h"

#include <vector.h>

TEST(ExampleTest, SimpleStatements) {
    EXPECT_TRUE(true);
    EXPECT_FALSE(false);
    EXPECT_EQ(true, true);
}

TEST(VectorTest, SimpleStatements) {
    Vector2D zero(0,0);
    EXPECT_EQ(zero.x, 0.f);
    EXPECT_EQ(zero.y, 0.f);
}
