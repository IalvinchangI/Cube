all: cube.exe

.PHONY: all clean

CC := gcc


# link
cube.exe: main.o vector.o input.o camera.o screen.o
	$(CC) $^ -o $@

# compile
main.o: main.c object.h vector.h input.h camera.h screen.h
	$(CC) -c $< -o $@

vector.o: vector.c vector.h
	$(CC) -c $< -o $@

input.o: input.c input.h __object.h
	$(CC) -c $< -o $@

camera.o: camera.c camera.h __object.h
	$(CC) -c $< -o $@

screen.o: screen.c screen.h __object.h
	$(CC) -c $< -o $@



### delete obj file
clean_obj:
	-@del *.o


### delete obj file and exe file
clean:
	-@del *.o
	-@del cube.exe
