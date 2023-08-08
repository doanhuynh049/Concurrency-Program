SRC := main_session1.cpp datagram_socket.cpp unix_socket.cpp
# SRC := $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

EXEC = run

PHONY = all
all: $(EXEC)
	./$(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $(EXEC) 

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@  

PHONY+= clean
clean: 
	rm -f $(OBJ) $(EXEC)

.PHONY: ${PHONY}