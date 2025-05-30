LEDALIB = '/usr/local/LEDA'
LEDAINCL = '/usr/local/LEDA/incl'
INCL = '../incl'

compile: main.o kruskal.o 
	g++ -O3 -o kruskal main.o kruskal.o -L$(LEDALIB) -lleda

run:
	./kruskal

clean:
	rm -f kruskal *.o

kruskal.o:  ../src/kruskal.cpp
	g++ -O3 -c ../src/kruskal.cpp -I$(INCL) -I$(LEDAINCL) 

main.o: ../src/main.cpp
	g++ -O3 -c ../src/main.cpp -I$(INCL) -I$(LEDAINCL)

