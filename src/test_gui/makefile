CC=g++
CFLAGS=-c -g -O2 `pkg-config --cflags gtk+-2.0` 
LIBS= `pkg-config --libs gtk+-2.0` 

gui:    gui.o
	$(CC) $(LIBS) -o $@ gui.o 

gui.o: gui.cpp
	      $(CC) $(CFLAGS) $< -o $@

clean:
	@rm -f  *.o *~ myma
