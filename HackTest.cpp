#include <cstdio>

#include "hack.h"

#define TEST_EQ(d, x, y) \
	do { \
		if (x == y) printf("[PASSED]\t%s\n", d); \
		else printf("[FAIL]\t%s at line %d: %s == %s failed\n", d, __LINE__, #x, #y); \
	} while (0);

#define TEST_TRUE(d, x) \
    do { \
        if (x) printf("[PASSED]\t%s\n", d); \
        else printf("[FAIL]\t%s at line %d\n", d, __LINE__); \
    } while (0);

using namespace std;

void Vector2DTest();
void Vector3DTest();

int main()
{
	Vector2DTest();
	Vector3DTest();
    MapTest();
	return 0;
}

// Vector2D test
void Vector2DTest()
{
	Vector2D a(1,0);
	Vector2D z(0,0);
	Vector2D b(3,-5);

	TEST_EQ("operator+", a+b, Vector2D(4,-5));
	TEST_EQ("binary operator-", a-b, Vector2D(-2,5));
	TEST_EQ("unary operator-", -a, Vector2D(-1,0));
	TEST_EQ("dot operator*", a*z, 0);
	TEST_EQ("scalar multiply operator*", b*1.5, Vector2D(4.5,-7.5));
	TEST_EQ("operator==", a==b, false);
	TEST_EQ("getNormalVector edge case", z.getNormalVector(), z);
	TEST_EQ("getNormalVector", a.getNormalVector(), Vector2D(0,1));
	
	// failed unit test
	// TEST_EQ("dummy test", a == b, true);
	return;
}

void Vector3DTest()
{
	Vector3D i(1,0,0);
	Vector3D j(0,1,0);
	Vector3D k(0,0,1);
	Vector3D z(0,0,0);

	Vector2D d(3,-5);
	TEST_EQ("constructor from Vector2D", Vector3D(d), Vector3D(3,-5,0));
	TEST_EQ("operator+", i+j, Vector3D(1,1,0));
	TEST_EQ("binary operator-", j-k, Vector3D(0,1,-1));
	TEST_EQ("unary operator-", -z, z);
	TEST_EQ("unary operator-", -i, Vector3D(-1,0,0));
	Vector3D u = i+j;
	u += k;
	TEST_EQ("operator+=", u, Vector3D(1,1,1));
	u = k;
	u -= j;
	TEST_EQ("operator-=", u, Vector3D(0,-1,1));
	u = Vector3D(1,2,3);
	u *= 3.14;
	TEST_EQ("operator*=", u, Vector3D(3.14, 6.28, 9.42));
	TEST_EQ("operator==", i==j, false);
	TEST_EQ("dot operator*", i*j, 0);
	TEST_EQ("scalar multiply operator*", j*1.5, Vector3D(0,1.5,0));
	return;
}

