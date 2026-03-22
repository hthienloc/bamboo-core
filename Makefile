CXX = g++
CXXFLAGS = -std=c++17 -I./include -I./src -Wall -Wextra -O3

SOURCES = src/engine/Character.cpp \
          src/engine/Spelling.cpp \
          src/engine/Rules.cpp \
          src/engine/StateManager.cpp \
          src/engine/Engine.cpp \
          src/api/EngineFactory.cpp

OBJECTS = $(SOURCES:.cpp=.o)

TEST_SOURCE = src/tests/main.cpp
COMP_TEST_SOURCE = src/tests/comprehensive_test.cpp
TEST_EXEC = bamboo_test
COMP_TEST_EXEC = comprehensive_test
INTER_TEST_EXEC = interactive_test

all: $(TEST_EXEC) $(COMP_TEST_EXEC) $(INTER_TEST_EXEC)

$(TEST_EXEC): $(OBJECTS) src/tests/main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

$(COMP_TEST_EXEC): $(OBJECTS) src/tests/comprehensive_test.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

$(INTER_TEST_EXEC): $(OBJECTS) src/tests/interactive_test.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(TEST_EXE)
