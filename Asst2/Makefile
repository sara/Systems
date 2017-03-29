CFLAGS = -ggdb -Wall

all: clean index

index: tokenizer.c tokenizer.h
	gcc $(CCFLAGS) tokenizer.c -o index

clean:
	rm -rf index
