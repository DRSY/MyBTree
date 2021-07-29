CC=g++
C_FLAGS=-std=c++17
SOURCE=btree.cpp

all: 
	$(CC) $(C_FLAGS) -o b-tree $(SOURCE)

clean:
	rm b-tree
