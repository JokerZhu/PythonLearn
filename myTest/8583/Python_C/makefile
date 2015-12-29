src = test.c
object = test.o
lib = test.so
bin = maintest

all : $(object)		\
		$(lib)		\
		$(bin)

$(object):$(src)
	cc  -c $^

$(lib):$(object)
	gcc -shared -o $@ $^ 
$(bin):main.c
	gcc -o $@ $^ $(object)

clean:
	rm $(object)
	rm $(lib)
	rm $(bin)

