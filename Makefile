LIBS=-lsfml-graphics -lsfml-window -lsfml-system

all:
	g++ -c ./source/main.cpp -std=c++11
	g++ main.o -o sfml-app $(LIBS) -std=c++11
	./sfml-app

clean:
	rm main.o sfml-app