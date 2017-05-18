CC = gcc
FLAGS = -Wall -g

build: readln
	$(CC) $(FLAGS) controller.c readln.o concat.o processInput.o -o controller
	$(CC) $(FLAGS) const.c readln.o -o const
	$(CC) $(FLAGS) filter.c readln.o processInput.o -o filter
	$(CC) $(FLAGS) window.c readln.o processInput.o -o window
	$(CC) $(FLAGS) spawn.c readln.o processInput.o concat.o -o spawn

readln: concat
	$(CC) $(FLAGS) -c readln.c

concat: processInput
	$(CC) $(FLAGS) -c concat.c

processInput:
	$(CC) $(FLAGS) -c processInput.c

clean:
	rm controller const filter window spawn *.o in* out*
