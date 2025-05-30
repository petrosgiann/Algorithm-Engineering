LEDALIB = '/usr/local/LEDA'
LEDAINCL = '/usr/local/LEDA/incl'
INCL = '../incl'

compile: main.o bellmanFord.o 
	g++ -O3 -o bellmanFord -std=c++0x main.o bellmanFord.o -lleda -L$(LEDALIB)

main.o: ../src/main.cpp
	g++ -O3 -c ../src/main.cpp -I$(INCL) -I$(LEDAINCL)

bellmanFord.o: ../src/bellmanFord.cpp
	g++ -O3 -c ../src/bellmanFord.cpp -I$(INCL) -I$(LEDAINCL)

clean:
	rm -f bellmanFord *.o

run:
	./bellmanFord
