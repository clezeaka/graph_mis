CC=gcc
CFLAGS=
CILK=g++
CILKFLAGS= -Wall -DCILKP -g -O2 -fcilkplus -lcilkrts -gdwarf-2
LDFLAGS= -L$(CURDIR)
#AR=ar
ifneq ($(CILKOFF),1)
	CFLAGS += $(CILKFLAGS)
endif

all: main

main : main.cpp tournament.cpp tournament.h graph.h utils.h graphUtils.h graphIO.h Makefile
	$(CILK) $(CFLAGS) main.cpp $(LDFLAGS) -o $@

clean :
	rm -f main *~

run :
	./run.sh $(file) $(type) $(nworkers)
