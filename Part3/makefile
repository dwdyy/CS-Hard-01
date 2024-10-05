.PHONY : clean
all : libmy_malloc.so my_malloc_test std_malloc_test data
libmy_malloc.so: my_malloc.c 
	gcc -w -shared -o libmy_malloc.so my_malloc.c -fPIC
my_malloc_test: my_malloc_test.c my_malloc.c
	gcc -w -o my_malloc_test my_malloc_test.c  my_malloc.c
std_malloc_test: std_malloc_test.c
	gcc -o std_malloc_test std_malloc_test.c
data : get_data.c
	gcc -o data get_data.c

clean : 
	-rm -f my_malloc_test
	-rm -f data
	-rm -f std_malloc_test
	-rm -f my_malloc.so
	-rm -f my.txt
	-rm -f std.txt
	-rm -f in.txt
	-rm -f all
	-rm -f libmy_malloc.so