CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -g

TARGET := atomc

SOURCES := main.cpp \
           frontend/lexer.cpp \
		   frontend/token.cpp \
		   frontend/parser.cpp

OBJECTS := $(SOURCES:.cpp=.o)
DEPS := $(OBJECTS:.o=.d)

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

test: $(TARGET)
	./$(TARGET) tests/basic.atom

clean:
	rm -f $(TARGET) $(OBJECTS) $(DEPS)

-include $(DEPS)