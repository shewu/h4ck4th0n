#include "gtest/gtest.h"

#define NUM_BAD_MAPS 13

TEST (MapTest, InvalidMaps) {
	for(int i = 1; i <= NUM_BAD_MAPS; i++) {
		char fname[30];
		strcpy(fname, "maps/badmap00.hbm")
		fname[11] = '0' + (char)(i / 10);
		fname[12] = '0' + (char)(i % 10);

		bool exc = false;

		HBMap hbmap;
		try {
			hbmap(fname);
		}
		catch(ParseException pe) {
			exc = true;
		}

		EXPECT_TRUE(exc);
	}
}
