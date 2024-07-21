CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I/usr/include
LDFLAGS = -L/usr/lib
LDLIBS = -lsfml-graphics -lsfml-window -lsfml-system
TARGET = simple_platformer_editor.exe
SRC = main.cpp
OBJ = $(SRC:.cpp=.o)

# Default target.
all: $(TARGET)

# Rule to build the target executable.
$(TARGET): $(OBJ)
	clear
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	./$(TARGET)

# Rule to build the object files.
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to run the program.
run: $(TARGET)
	./$(TARGET)

# Rule to clean the build directory.
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean run
