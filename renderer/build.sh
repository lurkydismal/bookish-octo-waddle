clear

clang++ -Og -g -flto=full -fuse-ld=mold -L/usr/local/lib  -std=c++17 -pthread -I../glad/include main.cpp ../glad/src/gl.c -lglfw -lfilament -lbackend -lbluegl -lbluevk -lfilabridge -lfilaflat -lutils -lgeometry -lsmol-v -lvkshaders -libl -lpthread -lc++ -ldl -lEGL -lX11 -o main
