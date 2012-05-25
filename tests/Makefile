TESTFLAGS=-I./tests/gtest/include -Ltests/gtest/lib -o test

test:
	$(CC) -o HackTest HackTest.cpp -I../ -L../
	./HackTest
	rm HackTest

test2:
	$(CC) test.cpp $(TESTFLAGS) -I../ -L../
	./test
	rm test
	
