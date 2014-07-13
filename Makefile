
CPPFLAGS = -Wall -Wextra -Weffc++ -std=c++11
LDFLAGS = -lz

all: sogno
	g++ -o flashtool main.cpp sognoHeader.o $(CPPFLAGS) $(LDFLAGS)

sogno:
	g++ -c sognoHeader.cpp $(CPPFLAGS)
