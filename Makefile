CC = gcc
FLAGS = -Wall -g

build: readln
	$(CC) $(FLAGS) src/controller.c readln.o concat.o processInput.o -o controller
	$(CC) $(FLAGS) src/const.c readln.o concat.o -o const
	$(CC) $(FLAGS) src/filter.c readln.o processInput.o -o filter
	$(CC) $(FLAGS) src/window.c readln.o processInput.o -o window
	$(CC) $(FLAGS) src/spawn.c readln.o processInput.o concat.o -o spawn

readln: concat
	$(CC) $(FLAGS) -c src/readln.c

concat: processInput
	$(CC) $(FLAGS) -c src/concat.c

processInput: clean
	$(CC) $(FLAGS) -c src/processInput.c

clean:
	$(RM) controller const filter window spawn *.o pipes/in* pipes/out* 
