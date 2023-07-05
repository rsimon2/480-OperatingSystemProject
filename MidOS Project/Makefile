os: system.o main.o
	g++ system.o main.o -o os -pthread

system.o: system.cpp system.h
	g++ -c system.cpp -pthread

main.o: main.cpp system.h
	g++ -c main.cpp -pthread

clean:
	rm -f system.o main.o os

run:
	./os 1568 prog1.txt prog2.txt prog3.txt prog1.txt idle.txt