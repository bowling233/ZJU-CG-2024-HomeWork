RM = rm -f
CXX = g++
CXXFLAGS = -Wall -g
# edit two lines below to point to your SOIL2 directory
INCLUDES = -I/river/sync/Code/_course/graphics/SOIL2/src
LDPATH = -lglfw -lGLEW -lGL -L/river/sync/Code/_course/graphics/SOIL2/cmake -lsoil2

all: main
	./main

utils.o: Utils.cpp Utils.h
	g++ -c Utils.cpp -std=c++11 $(INCLUDES)

sphere.o: Sphere.cpp Sphere.h
	g++ -c Sphere.cpp -std=c++11 $(INCLUDES)

main: utils.o sphere.o main.cpp
	g++ -o main main.cpp Utils.o Sphere.o -std=c++11 $(INCLUDES) $(LDPATH)

clean:
	rm -f *.o main
