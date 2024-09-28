all: ppm.cpp
	g++ ppm.cpp -o ppm -std=c++11 -Wall -g

run: all
	./ppm

clean:
	rm -rf ppm.dSYM ppm test.ppm

zip: ppm.cpp Makefile README test.ppm
	zip -r 3220106026_hw1.zip ppm.cpp Makefile README test.ppm