WARNING = -Wall -Wshadow --pedantic
ERROR = -Wvla -Werror -Wno-unused-variable  -Wno-unused-value
GCC = gcc -std=c99 -g $(WARNING) $(ERROR)
VAL = valgrind --tool=memcheck --log-file=memcheck.txt --leak-check=full --show-leak-kinds=all --verbose

SRCS = main.c a8.c
OBJS = $(SRCS:%.c=%.o)

a5: $(OBJS)
	$(GCC) $(OBJS) -o a8 -lm

.c.o:
	$(GCC) -c $*.c

test_43: a8
	$(VAL) ./a8 ./A8_testcases/Graph/input43.txt 

clean: # remove all machine generated files
	rm -f a8 *.o output? *~ memcheck.txt