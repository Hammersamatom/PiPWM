CC=g++
CPPFLAGS=-march=native -mtune=native -Os -fno-plt -flto -fno-exceptions -lwiringPi
PROGNAME=pipwm

$(PROGNAME): main.o
	$(CC) -o $(PROGNAME) main.o $(CPPFLAGS)

strip:
	strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag $(PROGNAME)


.PHONY: clean
clean:
	rm $(PROGNAME) *.o *.out
