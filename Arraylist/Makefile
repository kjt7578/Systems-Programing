test: arraylist.o test.o
	gcc	arraylist.o	test.o	-o	test

test.o:	test.c
	gcc	-c	test.c
	
arraylist.o:	arraylist.c	
#arraylist.h 도 자기가 알아서 메이킹함
	gcc	-c	arraylist.c
	
clean:
	rm	*.o	test
