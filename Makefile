CC = gcc
FLAGS = -Wall -g

build: const
	$(CC) $(FLAGS) controller.c readln.o concat.o -o controller
	$(CC) $(FLAGS) const.c readln.o -o const

const: readln
	$(CC) $(FLAGS) -c const.c

readln: concat
	$(CC) $(FLAGS) -c readln.c

concat:
	$(CC) $(FLAGS) -c concat.c

clean:
	rm  controller fifo* *.o
