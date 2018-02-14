CC=g++
CFLAGS=-g -O3 --std=c++11

../marker/run: ref_solution.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f *.o ../marker/run
