TARGETS = sikradio-sender

CC     = gcc
CFLAGS = -Wall -O2
LFLAGS = -Wall

all: $(TARGETS)

sikradio-sender.o err.o: err.h

sikradio-sender: sikradio-sender.o err.o

.PHONY: clean

clean:
	rm -f $(TARGETS) *.o *~ *.bak
