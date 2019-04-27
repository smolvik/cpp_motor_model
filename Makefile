SYSCONF_LINK = g++
CPPFLAGS     = -g -O0
LDFLAGS      = 	
LIBS         = -lm

all: ac tr sim

HEADERS := $(wildcard *.h)

tst:
	 @echo $(HEADERS)

sim: sim.cpp $(HEADERS)
	$(SYSCONF_LINK) -o sim sim.cpp -lplotter -lXaw -lXmu -lXt -lSM -lICE -lXext -lX11 -lpng -lz -lm

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
	-rm -f *.tga *.o tr ac sim
