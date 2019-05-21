SYSCONF_LINK = g++
CPPFLAGS     = -Wall -g -O0
LDFLAGS      = 	
LIBS         = -lm

all: ac tr sim test

HEADERS := $(wildcard *.h)

test: tgaplotter.o tgaimage.o test.cpp $(HEADERS)
	$(SYSCONF_LINK) -Wall $(LDFLAGS) -o $@ tgaplotter.o tgaimage.o test.cpp $(LIBS)

sim: sim.cpp $(HEADERS)
	$(SYSCONF_LINK) $(CPPFLAGS) -o sim sim.cpp -lplotter -lXaw -lXmu -lXt -lSM -lICE -lXext -lX11 -lpng -lz -lm -lpthread

ac: tgaplotter.o tgaimage.o ac.cpp $(HEADERS)
	$(SYSCONF_LINK) -Wall $(LDFLAGS) -o $@ tgaplotter.o tgaimage.o ac.cpp $(LIBS)

tr: tgaplotter.o tgaimage.o tr.cpp $(HEADERS)
	$(SYSCONF_LINK) -Wall $(LDFLAGS) -o $@ tgaplotter.o tgaimage.o tr.cpp $(LIBS)

tgaplotter.o: tgaplotter.cpp $(HEADERS)
	$(SYSCONF_LINK) -Wall $(CPPFLAGS) -c $(CFLAGS) $< -o $@
	
tgaimage.o: tgaimage.cpp $(HEADERS)
	$(SYSCONF_LINK) -Wall $(CPPFLAGS) -c $(CFLAGS) $< -o $@	

doc:
	doxygen

clean:
	-rm -f *.tga *.o tr ac sim test
