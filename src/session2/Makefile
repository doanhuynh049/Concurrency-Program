SRC = main.cpp multiprocessing.cpp
OBJ = $(SRC:.cpp=.o)

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

EXEC = run

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $(EXEC) 

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@  

clean: 
	rm -f $(OBJ) $(EXEC)

.PHONY: clean all