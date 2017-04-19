main: main.o util.o more.o jobs.o
	gcc -lm -O2 -o simpleshell main.o util.o more.o jobs.o

main.o: main.c util.h
	gcc -c -Wall -std=gnu99 -O2 main.c

util.o: util.c util.h more.h jobs.h
	gcc -c -Wall -std=gnu99 -O2 util.c

more.o: more.c more.h 
	gcc -c -Wall -std=gnu99 -O2 more.c

jobs.o: jobs.c jobs.h 
	gcc -c -Wall -std=gnu99 -O2 jobs.c

clean:
	rm simpleshell main.o util.o more.o jobs.o