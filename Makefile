# Makefile for UD CISC user-level thread library

CC = gcc
CFLAGS = -g

LIBOBJS = t_lib.o 

TSTOBJS = test00.o t1.o t2.o

# specify the executable 

EXECS = test00

# specify the source files

LIBSRCS = t_lib.c

TSTSRCS = test00.c


# Make all the tests
all: t1 t1x t3 t10 diningPhilosophers shone

# ar creates the static thread library

t_lib.a: ${LIBOBJS} Makefile
	ar rcs t_lib.a ${LIBOBJS}

# here, we specify how each file should be compiled, what
# files they depend on, etc.

t_lib.o: t_lib.c t_lib.h Makefile
	${CC} ${CFLAGS} -c t_lib.c

test00.o: test00.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c test00.c

test00: test00.o t_lib.a Makefile
	${CC} ${CFLAGS} test00.o t_lib.a -o test00

t1.o: t1.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c t1.c

t1: t1.o t_lib.a Makefile
	${CC} ${CFLAGS} t1.o t_lib.a -o t1

t1x.o: t1x.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c t1x.c

t1x: t1x.o t_lib.a Makefile
	${CC} ${CFLAGS} t1x.o t_lib.a -o t1x

t3.o: t3.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c t3.c

t3: t3.o t_lib.a Makefile
	${CC} ${CFLAGS} t3.o t_lib.a -o t3

t10.o: t10.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c t10.c

t10: t10.o t_lib.a Makefile
	${CC} ${CFLAGS} t10.o t_lib.a -o t10

diningPhilosophers.o: diningPhilosophers.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c diningPhilosophers.c

diningPhilosophers: diningPhilosophers.o t_lib.a Makefile
	${CC} ${CFLAGS} diningPhilosophers.o t_lib.a -o diningPhilosophers

shone.o: shone.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c shone.c

shone: shone.o t_lib.a Makefile
	${CC} ${CFLAGS} shone.o t_lib.a -o shone

clean:
	rm -f t_lib.a ${EXECS} ${LIBOBJS} ${TSTOBJS} *.o t1 t1x t3 t10 shone diningPhilosophers