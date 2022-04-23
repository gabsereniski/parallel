linear:
	gcc -c timer.c
	gcc linear.c timer.o -o linear.exe

static:
	gcc -c timer.c
	gcc -pthread static.c timer.o -o static.exe

dynamic:
	gcc -c timer.c
	gcc -pthread dynamic.c timer.o -o dynamic.exe

clean:
	rm *.o
	rm *.exe