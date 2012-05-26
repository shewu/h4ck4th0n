GTEST_DIR=./gtest
GTESTFLAGS=-I${GTEST_DIR}/include ${GTEST_DIR}/libgtest.a ${GTEST_DIR}/libgtest_main.a

test:
	$(CC) -o HackTest HackTest.cpp -I../ -L../
	./HackTest
	rm HackTest

test2:
	$(CC) -o test $(GTESTFLAGS) test.cpp -I../ -L../
	./test
	rm test
	
