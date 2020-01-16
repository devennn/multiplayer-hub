CFLAGS = -Wall -pedantic -std=gnu99

.PHONY: all careful extra

.DEFAULT_GOAL := all

all: 2310hub 2310alice 2310bob clean

careful: CFLAGS += -lm -g

#hub
2310hub: hub.o hubsrc.o
	gcc hub.o hubsrc.o -lm -o 2310hub
hub.o: hub.c hubhdr.h struct.h
	gcc $(CFLAGS) -c hub.c hubhdr.h struct.h
hubsrc.o: hubsrc.c hubhdr.h struct.h
	gcc $(CFLAGS) -c hubsrc.c hubhdr.h struct.h

#alice
2310alice: alice.o alicesrc.o
	gcc alice.o alicesrc.o -lm -o 2310alice
alice.o: alice.c struct.h
	gcc $(CFLAGS) -c alice.c struct.h
alicesrc.o: alicesrc.c alicehdr.h struct.h
	gcc $(CFLAGS) -c alicesrc.c alicehdr.h struct.h

#bob
2310bob: bob.o bobsrc.o
	gcc bob.o bobsrc.o -lm -o 2310bob
bob.o: bob.c struct.h
	gcc $(CFLAGS) -c bob.c struct.h
bobsrc.o: bobsrc.c bobhdr.h struct.h
	gcc $(CFLAGS) -c bobsrc.c bobhdr.h struct.h

clean:
	rm *.o
	rm *.gch
