GTEST_DIR = ./gtest
HOLYBALLS_DIR = ..

CPPFLAGS += -I$(GTEST_DIR)/include

CXXFLAGS += -g -Wall -Wextra

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

test: gtest_main.a test.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -v -L$(HOLYBALLS_DIR) $^ -o $@
	./test

test.o: test.cpp $(GTEST_HEADERS)
	$(CXX) -I$(HOLYBALLS_DIR) $(CPPFLAGS) $(CXXFLAGS) -c test.cpp

#################################
# Builds gtest.a and gtest_main.a
#################################

GTEST_SRCS_=$(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

gtest-all.o: $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o: $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c $(GTEST_DIR)/src/gtest_main.cc

gtest.a: gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a: gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

clean:
	@rm test
