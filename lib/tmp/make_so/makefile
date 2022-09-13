libmymath.so:f1.o f2.o
	gcc -shared -o $@ $^
%.o:%.c
	gcc -fPIC -c $<
.PHONY:clean
clean:
	rm *.o libmymath.so
	rm -fr output
.PHONY:output
output:
	mkdir output
	cp libmymath.so output
	cp *.h output
