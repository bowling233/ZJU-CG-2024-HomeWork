RM = rm -f
CXX = g++
CXXFLAGS = -Wall -g
# edit two lines below to point to your SOIL2 directory
INCLUDES = -I/river/sync/Code/_course/graphics/SOIL2/src
LDPATH	= -lglfw -lGLEW -lGL -L/river/sync/Code/_course/graphics/SOIL2/cmake -lsoil2
C_SRC	= ImportedModel.cpp Utils.cpp Sphere.cpp
OBJ	= $(patsubst %.cpp,%.o,$(C_SRC))

all: clean main
	./main

%.o:%.cpp
	${CXX} ${CXXFLAG} -c $< ${INCLUDES} ${LDPATH}

main: $(OBJ)
	g++ -o main main.cpp ${OBJ} -std=c++11 $(INCLUDES) $(LDPATH)

clean:
	rm -f *.o main
