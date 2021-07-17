CFLAGS+= -g

OBJ := bkey_parse.o

all: bkey_parse

bkey_parse: $(OBJ)
	$(CC) -o $@ $^

clean:
	rm -rf *.o
	rm -rf bkey_parse
