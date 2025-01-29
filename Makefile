all: fill.exe

.PHONY: all clean

CC := gcc


# link
fill.exe: fill.o vector.o screen.o
	$(CC) $^ -o $@

# compile
fill.o: fill.c fill.h vector.h screen.h
	$(CC) -c $< -o $@

vector.o: vector.c vector.h
	$(CC) -c $< -o $@

screen.o: screen.c screen.h __object.h
	$(CC) -c $< -o $@



### delete obj file
clean_obj:
	-@del *.o


### delete obj file and exe file
clean:
	-@del *.o
	-@del fill.exe
