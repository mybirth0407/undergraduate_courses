CC=g++
CFLAGS=-g -O3 --std=c++11

run: solution.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f *.o run
