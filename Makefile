SYSCONF_LINK = g++
CPPFLAGS     = -g -O0
LDFLAGS      = 	
LIBS         = -lm

all: ac tr

HEADERS := $(wildcard *.h)

tst:
	 @echo $(HEADERS)

ac: plotter.o tgaimage.o ac.cpp $(HEADERS)
	$(SYSCONF_LINK) -Wall $(LDFLAGS) -o $@ plotter.o tgaimage.o ac.cpp $(LIBS)

tr: plotter.o tgaimage.o tr.cpp $(HEADERS)
	$(SYSCONF_LINK) -Wall $(LDFLAGS) -o $@ plotter.o tgaimage.o tr.cpp $(LIBS)

plotter.o: plotter.cpp $(HEADERS)
	$(SYSCONF_LINK) -Wall $(CPPFLAGS) -c $(CFLAGS) $< -o $@
	
tgaimage.o: tgaimage.cpp $(HEADERS)
	$(SYSCONF_LINK) -Wall $(CPPFLAGS) -c $(CFLAGS) $< -o $@	

clean:
	-rm -f *.tga *.o tr ac
