CC = gcc
FLAGS = -Wall -g

build: const
	$(CC) $(FLAGS) controller.c readln.o concat.o processInput.o -o controller
	$(CC) $(FLAGS) const.c readln.o -o const

const: readln
	$(CC) $(FLAGS) -c const.c

readln: concat
	$(CC) $(FLAGS) -c readln.c

concat: processInput
	$(CC) $(FLAGS) -c concat.c

processInput:
	$(CC) $(FLAGS) -c processInput.c

clean:
	rm  controller *.o in* out*
