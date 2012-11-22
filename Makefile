CC = g++
INCLUDES = -include /usr/include/errno.h
FLAGS = -DDEBUG

all:
	$(CC) $(FLAGS) $(INCLUDES) -o trident Parser.cpp TrIdEnT.cpp -lreadline
