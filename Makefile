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

TARGETS = sikradio-sender

CC     = g++
CFLAGS = -Wall -O2 -std=c++17

all: $(TARGETS)

sikradio-sender: sikradio-sender.o err.o
	$(CXX) $(CXXFLAGS) $^ -o $@ -lboost_program_options

.PHONY: clean

clean:
	rm -f $(TARGETS) *.o *~ *.bak
