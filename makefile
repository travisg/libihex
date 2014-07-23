all: libihex.a ihextobin

libihex.a: ihex.o
	$(AR) rcs $@ $^

ihextobin: ihextobin.o libihex.a
	$(CXX) -o $@ $^

clean:
	rm -f *.o ihexlib.a ihextobin

%.o: %.cpp $(wildcard *.h)
	$(CXX) -O2 -g -Wall -std=c++11 -c $< -o $@
