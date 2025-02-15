# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -mconsole

# Target executable
TARGET = AVL_Database.exe

# Source and header files
SRC = AVL_Database.cpp db_driver.cpp
HDR = AVL_Database.hpp

# Object files
OBJ = $(SRC:.cpp=.o)

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

# Compile source files into object files
%.o: %.cpp $(HDR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJ) $(TARGET)

# Run the executable
run: $(TARGET)
	./$(TARGET)
