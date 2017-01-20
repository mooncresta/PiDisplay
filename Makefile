CXXFLAGS=-Wall -O3 -g -fno-strict-aliasing $(RGB_INCDIR)
BINARIES=PiDisplay
OBJECTS=PiDisplay.o bridge.o gfx_core.o utils.o time.o panel_nitelite.o panel_normal.o panel_pacman.o panel_pong.o\
	panel_message.o panel_weather.o

RGB_LIB_DISTRIBUTION=rpi-rgb-led-matrix
RGB_LIBDIR=$(RGB_LIB_DISTRIBUTION)/lib
RGB_LIBRARY_NAME=rgbmatrix
RGB_LIBRARY=$(RGB_LIBDIR)/lib$(RGB_LIBRARY_NAME).a
LDFLAGS+=-L$(RGB_LIBDIR) -l$(RGB_LIBRARY_NAME) -lrt -lm -lpthread

# Where our library resides. It is split between includes and the binary
# library in lib
INC1=./include
INC2=./rpi-rgb-led-matrix/include
INC3=.
RGB_INCDIR=-I $(INC1) -I $(INC2)
RGB_LIBRARY_NAME=rgbmatrix
RGB_LIBRARY=$(RGB_LIBDIR)/lib$(RGB_LIBRARY_NAME).a
LDFLAGS+=-L$(RGB_LIBDIR) -l$(RGB_LIBRARY_NAME) -lrt -lm -lpthread

all : $(BINARIES)

test : test.o bridge.o gfx_core.o utils.o time.o panel_message.o $(RGB_LIBRARY)
	$(CXX) $(RGB_INCDIR) $(CXXFLAGS) test.o bridge.o gfx_core.o utils.o time.o panel_message.o -o $@ $(LDFLAGS)

PiDisplay : $(OBJECTS) $(RGB_LIBRARY)
	$(CXX) $(RGB_INCDIR) $(CXXFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

# (FYI: Make sure, there is a TAB-character in front of the $(MAKE))
$(RGB_LIBRARY):
	$(MAKE) -C $(RGB_LIBDIR)

%.o : %.cc
	$(CXX) $(RGB_INCDIR) $(CXXFLAGS) -DADAFRUIT_RGBMATRIX_HAT -c -o $@ $<

clean:
	rm -f *.o $(OBJECTS) $(BINARIES)
	$(MAKE) -C lib clean
