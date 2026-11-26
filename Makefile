CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -g

TARGET := atomc

SOURCES := main.cpp \
           frontend/lexer.cpp \
		   frontend/token.cpp \
		   frontend/parser.cpp \
		   frontend/ast_printer.cpp \
		   frontend/semantic.cpp \
		   middleend/ir.cpp \
		   middleend/ir_printer.cpp \
		   middleend/ir_generator.cpp \
		   middleend/ir_verifier.cpp \
		   middleend/cfg.cpp \
		   middleend/cfg_printer.cpp \
		   middleend/constant_folding.cpp \
		   middleend/constant_propagation.cpp \
		   middleend/dead_code_elimination.cpp
 
OBJECTS := $(SOURCES:.cpp=.o)
DEPS := $(OBJECTS:.o=.d)

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

test: $(TARGET)
	bash tests/run_tests.sh

clean:
	rm -f $(TARGET) $(OBJECTS) $(DEPS)

-include $(DEPS)