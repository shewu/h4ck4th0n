#include <cstdio>
#include "hack.h"

#define TEST_EQ(x, y) if(x == y) printf("[PASSED] %s == %s\n",#x,#y); \
						else printf("[FAIL] %s == %s\n",#x,#y);

using namespace std;

// Vector2D test
void Vector2DTest()
{
	Vector2D a(1,0);
	Vector2D z(0,0);
	Vector2D b(3,-5);

	TEST_EQ(a+b, Vector2D(4,-5));
	TEST_EQ(a-b, Vector2D(-2,5));
	TEST_EQ(-a, Vector2D(-1,0));
	TEST_EQ(a*z, 0);
	TEST_EQ(a==b, false);
	TEST_EQ(b*1.5, Vector2D(4.5,-7.5));
	return;
}

int main()
{
	Vector2DTest();
	return 0;
}

