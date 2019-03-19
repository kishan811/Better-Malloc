#!/bin/bash
rm -f main main.o mem.o
g++ -c -w -ggdb3 mem.cpp
g++ -c -w -ggdb3 main.cpp 
g++ -o main main.o mem.o
./main




