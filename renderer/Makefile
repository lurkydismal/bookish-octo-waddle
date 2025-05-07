FILAMENT_LIBS=-lfilament -lbackend -lbluegl -lbluevk -lfilabridge -lfilaflat -lutils -lgeometry -lsmol-v -lvkshaders -libl
CC=clang++

main: main.o
	$(CC) -Llib/x86_64/ main.o $(FILAMENT_LIBS) -lpthread -lc++ -ldl -o main

main.o: main.cpp
	$(CC) -Iinclude/ -std=c++17 -pthread -c main.cpp

clean:
	rm -f main main.o

.PHONY: clean
