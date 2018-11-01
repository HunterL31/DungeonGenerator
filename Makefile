LIBS=-lsfml-graphics -lsfml-window -lsfml-system

all:
	g++ -c ./source/main.cpp
	g++ main.o -o sfml-app $(LIBS)
	./sfml-app