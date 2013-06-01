
all: test nanosleepmin


test: test.c spinlock.h ticketlock.h rwlock.h
	gcc -O2 -Wall -pedantic -std=gnu99 -DNDEBUG test.c -pthread -o $@

nanosleepmin: nanosleepmin.c
	gcc -O2 -Wall -pedantic -std=gnu99 -DNDEBUG nanosleepmin.c -o $@

