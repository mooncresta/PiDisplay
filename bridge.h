// bridge.h

//#include "Adafruit_mfGFX.h"
#include "led-matrix.h" // Hardware-specific library
#include "graphics.h"
#include "canvas.h"
#include "trng.h"
#define boolean bool
extern unsigned int millis            (void) ;
int random(int, int);
static uint64_t epochMilli, epochMicro ;


namespace rgb_matrix {
//void drawFastVLine(Canvas *canvas, int16_t x, int16_t y, int16_t h, Color color);
//void drawFastHLine(Canvas *canvas, int16_t x, int16_t y, int16_t w, Color color);
void drawRect(Canvas *canvas, int16_t x, int16_t y, int16_t w, int16_t h, Color color);
void fillRect(Canvas *canvas, int16_t x, int16_t y, int16_t w, int16_t h, Color color);
void drawBitmap(Canvas *canvas, int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, Color color); 
void drawPixel(Canvas *canvas, int16_t x, int16_t y, Color color); 
void setTextSize(Canvas *canvas, int16_t s); 
void setTextWrap(Canvas *canvas, bool f); 
void setTextColor(Canvas *canvas, Color c); 
void setCursor(Canvas *canvas, int16_t x, int16_t y); 
void fillCircle(Canvas *canvas, int16_t x0, int16_t y0, int16_t r, Color color);
void fillCircleHelper(Canvas *canvas, int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, Color color);

};

class TimeClass {
public:
	// Arduino time and date functions
	static int     hour();            			// current hour
	static int     hour(time_t t);				// the hour for the given time
	static int     hourFormat12();    			// current hour in 12 hour format
	static int     hourFormat12(time_t t);		// the hour for the given time in 12 hour format
	static uint8_t isAM();            			// returns true if time now is AM
	static uint8_t isAM(time_t t);    			// returns true the given time is AM
	static uint8_t isPM();            			// returns true if time now is PM
	static uint8_t isPM(time_t t);    			// returns true the given time is PM
	static int     minute();          			// current minute
	static int     minute(time_t t);  			// the minute for the given time
	static int     second();          			// current second
	static int     second(time_t t);  			// the second for the given time
	static int     day();             			// current day
	static int     day(time_t t);     			// the day for the given time
	static int     weekday();         			// the current weekday
	static int     weekday(time_t t); 			// the weekday for the given time
	static int     month();           			// current month
	static int     month(time_t t);   			// the month for the given time
	static int     year();            			// current four digit year
	static int     year(time_t t);    			// the year for the given time
	static time_t  now();              			// return the current time as seconds since Jan 1 1970
	static void    zone(float GMT_Offset);		// set the time zone (+/-) offset from GMT
	static void    setTime(time_t t);			// set the given time as unix/rtc time
};

extern TimeClass Time;	//eg. usage: Time.day();

class RGBmatrixPanel  {

 public:

  // Constructor for 16x32 panel:
  RGBmatrixPanel(uint8_t a, uint8_t b, uint8_t c,
    uint8_t sclk, uint8_t latch, uint8_t oe, boolean dbuf, uint8_t width=32);

  // Constructor for 32x32 panel (adds 'd' pin):
  RGBmatrixPanel(uint8_t a, uint8_t b, uint8_t c, uint8_t d,
    uint8_t sclk, uint8_t latch, uint8_t oe, boolean dbuf, uint8_t width=32);

  void
    begin(void),
    drawPixel(int16_t x, int16_t y, uint16_t c),
    fillScreen(uint16_t c),
    updateDisplay(void),
    swapBuffers(boolean),
    dumpMatrix(void);
  uint8_t
    *backBuffer(void);
  uint16_t
    Color333(uint8_t r, uint8_t g, uint8_t b),
    Color444(uint8_t r, uint8_t g, uint8_t b),
    Color888(uint8_t r, uint8_t g, uint8_t b),
    Color888(uint8_t r, uint8_t g, uint8_t b, boolean gflag),
    ColorHSV(long hue, uint8_t sat, uint8_t val, boolean gflag);

 private:

  uint8_t         *matrixbuff[2];
  uint8_t          nRows;
  volatile uint8_t backindex;
  volatile boolean swapflag;

  // Init/alloc code common to both constructors:
  void init(uint8_t rows, uint8_t a, uint8_t b, uint8_t c,
    uint8_t sclk, uint8_t latch, uint8_t oe, boolean dbuf,
    uint8_t width);

  uint8_t	_sclk, _latch, _oe, _a, _b, _c, _d;

    //void debugpanel(String message, int value);

  // Counters/pointers for interrupt handler:
  volatile uint8_t row, plane;
  volatile uint8_t *buffptr;
};
