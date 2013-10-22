all: test_getter test_setter

test_getter: test_getter.c getter.c
	cc -o test_getter test_getter.c getter.c -lpthread

test_setter: test_setter.c setter.c
	cc -o test_setter test_setter.c setter.c -lpthread
