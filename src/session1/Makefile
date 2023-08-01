SRC = main_session2.cpp stream_socket.cpp
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