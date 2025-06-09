CXX = g++
CXXFLAGS = -std=c++14 -O2 -Iexternal/lammpstrj/include

all: cluster-analyze

cluster-analyze: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o $@

.PHONY: clean

clean:
	rm -f cluster-analyze
