CC=g++
CXXFLAGS=-march=native -mtune=native -Os -fno-plt -flto -fno-exceptions
LDFLAGS=-lwiringPi -lpthread
DEBUG =-Wall -Wextra -Werror -pedantic
PROGNAME=pipwm

$(PROGNAME): main.o
	$(CC) -o $(PROGNAME) main.o $(CXXFLAGS) $(DEBUG) $(LDFLAGS)

strip:
	strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag $(PROGNAME)


.PHONY: clean
clean:
	rm $(PROGNAME) *.o *.out
