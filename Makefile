OPTS=-Wall -g -std=c++11
#OPTS=-Wall -g -pg -std=c++11 -O
#OPTS=-Wall -std=c++11 -O3
CC=g++
#LIBS = -lm
LIBS = -L/home/cg/palantir/levmar/levmar-2.5 -llevmar -lm -lboost_filesystem -lboost_system
DEPS = -I includes/
SRC = $(wildcard src/*.cpp)
1776: 1776.cpp eriolObjs.o $(SRC)
	$(CC) $(OPTS) 1776.cpp eriolObjs.o $(SRC) ${DEPS} $(LIBS) -o $@
