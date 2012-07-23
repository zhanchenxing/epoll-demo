a.out: main.o util.o
	g++ -o a.out main.o util.o
main.o: main.cpp
	g++ -c main.cpp

util.o:	util.cpp
	g++ -c util.cpp

clear:
	rm *.o
	rm a.out

