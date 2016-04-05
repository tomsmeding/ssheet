uname_str := $(shell uname)
ifeq ($(uname_str),Darwin)
	CXX = g++-g
else
	CXX = /vol/share/software/gcc/5.3.0/bin/g++ -Wl,--rpath=/vol/share/software/gcc/5.3.0/lib64
endif
CXXFLAGS = -Wall -Wextra -std=c++14 -O2
LDFLAGS = -lncurses
BIN = main

obj_files = $(patsubst %.cpp,%.o,$(wildcard *.cpp))


.PHONY: all clean remake test

all: $(BIN)

clean:
	rm -f $(BIN) *.o

remake: clean all

test: all
	python3 modeltest.py


$(BIN): $(obj_files)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp *.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<
