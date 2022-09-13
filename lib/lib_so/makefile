test:test.c
	gcc -o test test.c -I ./lib -L ./lib -lmymath \
	-Wl,-rpath="/home/iiwish/linux_learn/lib/lib_so/lib"
.PHONY:clean
clean:
	rm test