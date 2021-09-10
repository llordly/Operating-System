LIBS = -lpthread

all: reader_writer philosophers

CC = gcc

reader_writer: reader_writer.c
		$(CC) -o $@ $< $(LIBS)

philosophers: philosophers.c
		$(CC) -o $@ $< $(LIBS)
