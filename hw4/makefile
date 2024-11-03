all: main
	./main

utils.o: Utils.cpp Utils.h
	g++ -c Utils.cpp -std=c++11

sphere.o: Sphere.cpp Sphere.h
	g++ -c Sphere.cpp -std=c++11

main: utils.o sphere.o main.cpp
	g++ -o main main.cpp Utils.o Sphere.o -std=c++11 -lGL -lGLEW -lglfw

clean:
	rm -f *.o main
