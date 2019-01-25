LIBS=-lsfml-graphics -lsfml-window -lsfml-system

all:
	g++ -c ./source/main.cpp -std=c++11
	g++ main.o -o Dungeon $(LIBS) -std=c++11

clean:
	rm main.o Dungeon