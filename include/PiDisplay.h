/********
* Main Include File
*
*********/

#include "led-matrix.h" // Hardware-specific library
#include "threaded-canvas-manipulator.h"
#include "graphics.h"
#include "fix_fft.h"
//#include "font3x5.h"
//#include "font5x5.h"
#include <iostream>
#include "string.h"
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "bridge.h"
#include "trng.h"


#ifndef PiDisplay_H
#define PiDisplay_H

using namespace rgb_matrix;
using namespace std;

#define DEBUGME
#define DEBUG(message) printf(message)
#define DEBUGp(message) printf(message)
#define DEBUGpln(message)

/********* Defines to enable Panels *****/
#define PANEL_PACMAN
#define PANEL_PONG
#define PANEL_MESSAGE
//#define PANEL_MARQUEE
//#define PANEL_WORDCLOCK
#define PANEL_WEATHER
//#define PANEL_XXXXX

#define K_WIDTH 64   // Number of Leds across
#define K_HEIGHT 32  // Number of rows/height

// Defines for Panel Sizes
// Message Panel - bottom half of screen 
#define K_MS_WIDTH K_WIDTH)
#define K_MS_HEIGHT (K_HEIGHT/2) // default to half the height
#define K_MS_X 0 // Start x of message panel
#define K_MS_Y (K_WIDTH/2) // Start y of message panel
// Weather Panel - top rh panel
#define K_W_WIDTH (K_WIDTH/2)
#define K_W_HEIGHT (K_HEIGHT/2) // default to half the height
#define K_W_X 0 // Start x of message panel
#define K_W_Y 16 // Start y of message panel

/*********** End of Defines *************/

/****************************************/

#define SHOWCLOCK 10000
#define MAX_CLOCK_MODE		7                 // Number of clock modes

/********** RGB565 Color definitions **********/
#define Black           0x0000
#define Navy            0x000F
#define DarkGreen       0x03E0
#define DarkCyan        0x03EF
#define Maroon          0x7800
#define Purple          0x780F
#define Olive           0x7BE0
#define LightGrey       0xC618
#define DarkGrey        0x7BEF
#define Blue            0x001F
#define Green           0x07E0
#define Cyan            0x07FF
#define Red             0xF800
#define Magenta         0xF81F
#define Yellow          0xFFE0
#define White           0xFFFF
#define Orange          0xFD20
#define GreenYellow     0xAFE5
#define Pink		0xF81F

// Colors 4/4/4
struct colourMap {
char *colour;
int r,g,b;
};
#define C_RED 0
#define C_LIME 1
#define C_BLUE 2
#define C_YELLOW 3
#define C_FUCHIA 4
#define C_AQUA 5
#define C_WHITE 6
#define C_BLACK 7
/**********************************************/
/********** PACMAN definitions **********/
//#define usePACMAN			// Uncomment to enable PACMAN animations

#define BAT1_X 2                         // Pong left bat x pos (this is where the ball collision occurs, the bat is drawn 1 behind these coords)
#define BAT2_X 28

#define X_MAX 63                         // Matrix X max LED coordinate (for 2 displays placed next to each other)
#define Y_MAX 15

/****************************/
/***** Weather webhook definitions *****/
#define HOOK_RESP		"hook-response/weather_hook"
#define HOOK_PUB		"weather_hook"
#define DEFAULT_CITY	"\"mycity\":\"Ottawa,ON\""	// Change to desired default city,state
#define API_KEY			"\"apikey\":\"yourkeyhere\""
#define UNITS			"\"units\":\"metric\""		// Change to "imperial" for farenheit units
/***************************************/

/************ PROTOTYPES **************/
int setMode(string command);
void quickWeather();
void getWeather();
void processWeather(const char *name, const char *data);
void showWeather();
void drawWeatherIcon(uint8_t x, uint8_t y, int id);
void pacClear();
void pacMan();
void drawPac(int x, int y, int z);
void drawGhost( int x, int y, Color color);
void drawScaredGhost( int x, int y);
void pong();
unsigned short pong_get_ball_endpoint(float tempballpos_x, float  tempballpos_y, float  tempballvel_x, float tempballvel_y);
void normal_clock();
void word_clock();
void jumble();
void display_date();
//void drawString(int x, int y, char* c,uint8_t font_size, uint16_t color);
//void drawString(int x, int y, char* c,uint8_t font_size, Color color);
//void drawChar(int x, int y, char c, uint8_t font_size, uint16_t color);
int calc_font_displacement(uint8_t font_size);
void spectrumDisplay();
void plasma();
void marquee();
void nitelite();
int timerEvaluate(const struct TimerObject on_time, const struct TimerObject off_time, const unsigned int currentTime);
time_t tmConvert_t(int YYYY, unsigned short MM, unsigned short DD, unsigned short hh, unsigned short mm, unsigned short ss);
void vectorNumber(int n, int x, int y, Color color, float scale_x, float scale_y);

namespace rgb_matrix {
//void drawFastVLine(Canvas *canvas, int16_t x, int16_t y, int16_t h, Color color);
//void drawFastHLine(Canvas *canvas, int16_t x, int16_t y, int16_t w, Color color);
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, Color color);
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, Color color);
void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, Color color);
void drawPixel(int16_t x, int16_t y, Color color);
void setTextSize(int s);
void setTextWrap(bool f);
void setTextColor(Color c);
void setCursor(int x, int y);
void drawCircle(int16_t x0, int16_t y0, int16_t r, Color color);
void fillCircle(int16_t x0, int16_t y0, int16_t r, Color color);
void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, Color color);
void  scrollBigMessage(char *m);
void flashing_cursor(unsigned short xpos, unsigned short ypos, unsigned short cursor_width, unsigned short cursor_height, unsigned short repeats);
void cls();
void drawString(int x, int y, char* c,uint8_t font_size, Color color);
void drawChar(int x, int y, char c, uint8_t font_size, Color color);
void swapBuffers(boolean copy);
void scrollMessage(char* top, char* bottom ,uint8_t top_font_size,uint8_t bottom_font_size, Color top_color, Color bottom_color);
};


struct t_FontLib {
        char name[20];
        int width;
        int height;
        Font *ptr;
};

#define F_SMALL 0
#define F_MEDIUM 1
#define F_LARGE 2

struct TimerObject{
  int hour, minute;
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

class MessageScroller : public ThreadedCanvasManipulator {
public:
  MessageScroller(RGBMatrix*, int, int, Color);
  virtual ~MessageScroller();

  void Run() ;
private:
  const int scroll_jumps_;
  const int scroll_ms_;

  int32_t horizontal_position_;

  RGBMatrix* matrix_;
  FrameCanvas* offscreen_;
};

extern volatile bool interrupt_received;
extern char* itoa(int a, char* buffer, unsigned char radix);
extern char g_scrolling_message[512];
extern TimeClass Time;	//eg. usage: Time.day();
extern Font mFont;
extern t_FontLib FontLib[];
extern int mode_changed;
extern uint16_t showClock;
extern unsigned long modeSwitch;
extern unsigned long updateCTime;
extern bool mode_quick;
extern int clock_mode;
extern RGBMatrix *matrix;
extern FrameCanvas *canvas; // Active Canvas
extern boolean wasWeatherShownLast;
extern unsigned long lastWeatherTime;
extern boolean weatherGood;
extern int badWeatherCall;

#endif
