CC=g++ -Wall

all: producer consumer start

start: start.o helper.o
	$(CC) -o start start.o helper.o

start.o: start.c helper.c
	$(CC) -c start.c helper.c

producer: producer.o helper.o
	$(CC) -o producer producer.o helper.o

producer.o: producer.c helper.c
	$(CC) -c producer.c helper.c

consumer: consumer.o helper.o
	$(CC) -o consumer consumer.o helper.o

consumer.o: consumer.c helper.c
	$(CC) -c consumer.c helper.c

tidy:
	rm -f *.o core

clean:
	rm -f start producer consumer *.o core
