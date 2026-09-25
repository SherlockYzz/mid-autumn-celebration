CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2 -I.

TARGET = festival_app.exe
TEST_TARGET = core_test.exe

SRC = festival/main.cpp
TEST_SRC = tests/core_standalone_test.cpp

all: $(TARGET) $(TEST_TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC)

$(TEST_TARGET): $(TEST_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_SRC)

clean:
	rm -f $(TARGET) $(TEST_TARGET) *.o

.PHONY: all clean
