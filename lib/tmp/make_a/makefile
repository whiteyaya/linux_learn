libmymath.a:f1.o f2.o
	ar -cr $@ $^
%.o:%.h
.PHONY:clean
clean:
	rm *.o libmymath.a
.PHONY:output
output:
	mkdir output
	cp libmymath.a output
	cp *.h output
