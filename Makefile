CXXFLAGS=-Wall -O3 -g -fno-strict-aliasing $(RGB_INCDIR)
BINARIES=PiDisplay

# Where our library resides. It is split between includes and the binary
# library in lib
INC1=./include
INC2=./rpi-rgb-led-matrix/include
INC3=.
RGB_INCDIR=-I $(INC1) -I $(INC2)
RGB_LIBDIR=lib
RGB_LIBRARY_NAME=rgbmatrix
RGB_LIBRARY=$(RGB_LIBDIR)/lib$(RGB_LIBRARY_NAME).a
LDFLAGS+=-L$(RGB_LIBDIR) -l$(RGB_LIBRARY_NAME) -lrt -lm -lpthread

all : $(BINARIES)

PiDisplay : PiDisplay.o $(RGB_LIBRARY)
	$(CXX) $(RGB_INCDIR) $(CXXFLAGS) PiDisplay.o -o $@ $(LDFLAGS)

%.o : %.cc
	$(CXX) $(RGB_INCDIR) $(CXXFLAGS) -DADAFRUIT_RGBMATRIX_HAT -c -o $@ $<

clean:
	rm -f *.o $(OBJECTS) $(BINARIES)
	$(MAKE) -C lib clean
