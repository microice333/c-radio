# TARGETS	= sikradio-sender
# CXX	= g++
# CXXFLAGS=  -std=c++17 -Wall -O2

# all: $(TARGETS) 

# err.o: err.h
# 	gcc

# sikradio-sender: sikradio-sender.cc err.o
# 	$(CXX) $(CXXFLAGS) $^ -o $@ -lboost_program_options


# .PHONY: clean TARGET
# clean:
# 	rm -f server client *.o *~ *.bak
# 	

TARGETS = sikradio-sender sikradio-reciver

CC     = g++
CFLAGS = -Wall -O2 -std=c++17

all: $(TARGETS)

sikradio-sender: sikradio-sender.o err.o
	$(CXX) $(CXXFLAGS) $^ -o $@ -lboost_program_options

sikradio-reciver: sikradio-reciver.o err.o
	$(CXX) $(CXXFLAGS) $^ -o $@ 

.PHONY: clean

clean:
	rm -f $(TARGETS) *.o *~ *.bak
