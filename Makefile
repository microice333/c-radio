TARGETS = sikradio-sender sikradio-reciver

CC     = gcc
CFLAGS = -Wall -O2 -std=c99
CXX = g++
CXXFLAGS = -Wall -O2 -std=c++17

all: $(TARGETS)

sikradio-sender: server.o err.o sikradio-sender.o   
	$(CXX) $(CXXFLAGS) $^ -o $@ -lboost_program_options -pthread

sikradio-reciver: sikradio-reciver.o err.o
	$(CXX) $(CXXFLAGS) $^ -o $@ 

.PHONY: clean

clean:
	rm -f $(TARGETS) *.o *~ *.bak
