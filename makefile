ihexlib.a: ihex.o
	$(AR) rcs $@ $^

clean:
	rm -f $(OBJS) ihexlib.a

%.o: %.cpp ihex.h
	$(CXX) -O2 -g -Wall -std=c++11 -c $< -o $@
