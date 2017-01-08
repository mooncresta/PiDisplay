/*
** Adapted and Enhanced by Mooncresta
*/

/*  RGB Pong Clock - Andrew Holmes @pongclock
**  Inspired by, and shamelessly derived from 
**      Nick's LED Projects
**  https://123led.wordpress.com/about/
**  
**  Uses an Adafruit 16x32 RGB matrix availble from here:
**  http://www.phenoptix.com/collections/leds/products/16x32-rgb-led-matrix-panel-by-adafruit
**  This microphone:
**  http://www.phenoptix.com/collections/adafruit/products/electret-microphone-amplifier-max4466-with-adjustable-gain-by-adafruit-1063
**  a DS1307 RTC chip (not sure where I got that from - was a spare)
** 
*/


//#include "Adafruit_mfGFX.h"   // Core graphics library
//#include "RGBmatrixPanel.h" // Hardware-specific library
#include "led-matrix.h" // Hardware-specific library
#include "graphics.h"
#include "fix_fft.h"
#include "blinky.h"
#include "font3x5.h"
#include "font5x5.h"
#include <iostream>
#include "string.h"
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "bridge.h"
#include "trng.h"

using rgb_matrix::GPIO;
using rgb_matrix::RGBMatrix;
using rgb_matrix::Color;
using rgb_matrix::Canvas;
using rgb_matrix::Font;
using namespace std;

#define DEBUGME


// allow us to use itoa() in this scope
extern char* itoa(int a, char* buffer, unsigned char radix);

#define		RGBPCversion	"V1.0"

#define DEBUG(message) printf(message)
#define DEBUGp(message) printf(message)
#define DEBUGpln(message)

/*
// Define RGB matrix panel GPIO pins 
#if defined (STM32F10X_MD)	//Core
	#define CLK D6
	#define OE  D7
	#define LAT A4
	#define A   A0
	#define B   A1
	#define C   A2
	#define D	A3
#endif
*/

/*
#if defined (STM32F2XX)	//Photon
	#define CLK D6
	#define OE  D7
	#define LAT A4
	#define A   A0
	#define B   A1
	#define C   A2
	#define D	A3
#endif
*/

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
/**********************************************/



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

const colourMap cMap[] = {
  {"RED", 255, 0,   0  },
  {"LIME",0, 255,   0  },
  {"BLUE",0, 0,   255  }, 
  {"YELLOW",255, 255,   0  },
  {"FUCHSIA",255, 0,   255  },
  {"AQUA",0, 255,  255  },
  {"WHITE",255, 255,   255  },
  {"BLACK",0, 0,   0  }
};

/********** PACMAN definitions **********/
//#define usePACMAN			// Uncomment to enable PACMAN animations

#define BAT1_X 2                         // Pong left bat x pos (this is where the ball collision occurs, the bat is drawn 1 behind these coords)
#define BAT2_X 28        

#define X_MAX 31                         // Matrix X max LED coordinate (for 2 displays placed next to each other)
#define Y_MAX 15

int powerPillEaten = 0;
              
/************* Using DST ?? ***************/

// Select North America or Central Europe...

//#define DST_NORTH_AMERICA
#define DST_CENTRAL_EUROPE

#if defined(DST_NORTH_AMERICA) || defined(DST_CENTRAL_EUROPE)
int summerOffset = -4;
int winterOffset = -5;
#endif


TimeClass Time;

struct SpecialDays {
  int month, day;
  char* message;
};

const SpecialDays ourHolidays[] = {  //keep message to < 40 chars
  { 1, 1, "HAPPY NEW YEAR"},
  { 2, 14, "HAPPY ST PATRICKS DAY"},
  { 4, 1, "HAPPY BIRTHDAY TOM"},

/********* USING SPECIAL MESSAGES **********/

#define USING_SPECIAL_MESSAGES

#ifdef USING_SPECIAL_MESSAGES
  { 7, 4, "HAPPY 4TH OF JULY"},
  { 7, 15, "HAPPY ANNIVERSARY"},
  { 8, 12, "HAPPY BIRTHDAY GRAMPA"},
  { 9, 26, "HAPPY BIRTHDAY CHASE AND CHANDLER"},
  {10, 31, "HAPPY HALLOWEEN"},
  {11, 1, "HAPPY BIRTHDAY PATCHES"},
  {12, 25, "MERRY CHRISTMAS"}
};
#endif

/***** Weather webhook definitions *****/
#define HOOK_RESP		"hook-response/weather_hook"
#define HOOK_PUB		"weather_hook"
#define DEFAULT_CITY	"\"mycity\":\"Ottawa,ON\""	// Change to desired default city,state
#define API_KEY			"\"apikey\":\"yourkeyhere\""
#define UNITS			"\"units\":\"metric\""		// Change to "imperial" for farenheit units
/***************************************/

/***** Create RGBmatrix Panel instance *****
 Last parameter = 'true' enables double-buffering, for flicker-free,
 buttery smooth animation.  Note that NOTHING WILL SHOW ON THE DISPLAY
 until the first call to swapBuffers().  This is normal. */
//RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, true, 32);	// 16x32
//RGBmatrixPanel matrix(A, B, C, D,CLK, LAT, OE, true, 32);	// 32x32
//RGBmatrixPanel matrix(A, B, C, D,CLK, LAT, OE, true, 64); // 64x32
/*******************************************/

int stringPos;
bool weatherGood(false);
int badWeatherCall;
char w_temp[8][7] = {""};
char w_id[8][4] = {""};

/*** Set default city for weather webhook ***/
char city[40] = DEFAULT_CITY;

bool wasWeatherShownLast(true);
unsigned long lastWeatherTime =0;

int mode_changed = 0;			// Flag if mode changed.
bool mode_quick = false;		// Quick weather display
int clock_mode = 0;				// Default clock mode (1 = pong)
uint16_t showClock = 300;		// Default time to show a clock face
unsigned long modeSwitch;
unsigned long updateCTime;		// 24hr timer for resyncing cloud time



/************  FFT definitions **********/
#define useFFT			// Uncomment to enable FFT clock

#if defined useFFT

// Define MIC input pin
#define MIC A5			// A7 for Core, A5 for Photon

int8_t im[128];
int8_t fftdata[128];
int8_t spectrum[32];

unsigned short
peak[32],		// Peak level of each column; used for falling dots
dotCount = 0,	// Frame counter for delaying dot-falling speed
colCount = 0;	// Frame counter for storing past column data

int8_t
col[32][10],	// Column levels for the prior 10 frames
minLvlAvg[32],	// For dynamic adjustment of low & high ends of graph,
maxLvlAvg[32];	// pseudo rolling averages for the prior few frames.
#endif
/***************************************/

/************  PLASMA definitions **********/

static const int8_t  sinetab[256] = {
     0,   2,   5,   8,  11,  15,  18,  21,
    24,  27,  30,  33,  36,  39,  42,  45,
    48,  51,  54,  56,  59,  62,  65,  67,
    70,  72,  75,  77,  80,  82,  85,  87,
    89,  91,  93,  96,  98, 100, 101, 103,
   105, 107, 108, 110, 111, 113, 114, 116,
   117, 118, 119, 120, 121, 122, 123, 123,
   124, 125, 125, 126, 126, 126, 126, 126,
   127, 126, 126, 126, 126, 126, 125, 125,
   124, 123, 123, 122, 121, 120, 119, 118,
   117, 116, 114, 113, 111, 110, 108, 107,
   105, 103, 101, 100,  98,  96,  93,  91,
    89,  87,  85,  82,  80,  77,  75,  72,
    70,  67,  65,  62,  59,  56,  54,  51,
    48,  45,  42,  39,  36,  33,  30,  27,
    24,  21,  18,  15,  11,   8,   5,   2,
     0,  -3,  -6,  -9, -12, -16, -19, -22,
   -25, -28, -31, -34, -37, -40, -43, -46,
   -49, -52, -55, -57, -60, -63, -66, -68,
   -71, -73, -76, -78, -81, -83, -86, -88,
   -90, -92, -94, -97, -99,-101,-102,-104,
  -106,-108,-109,-111,-112,-114,-115,-117,
  -118,-119,-120,-121,-122,-123,-124,-124,
  -125,-126,-126,-127,-127,-127,-127,-127,
  -128,-127,-127,-127,-127,-127,-126,-126,
  -125,-124,-124,-123,-122,-121,-120,-119,
  -118,-117,-115,-114,-112,-111,-109,-108,
  -106,-104,-102,-101, -99, -97, -94, -92,
   -90, -88, -86, -83, -81, -78, -76, -73,
   -71, -68, -66, -63, -60, -57, -55, -52,
   -49, -46, -43, -40, -37, -34, -31, -28,
   -25, -22, -19, -16, -12,  -9,  -6,  -3
};

const float radius1  = 65.2, radius2  = 92.0, radius3  = 163.2, radius4  = 176.8,
            centerx1 = 64.4, centerx2 = 46.4, centerx3 =  93.6, centerx4 =  16.4, 
            centery1 = 34.8, centery2 = 26.0, centery3 =  56.0, centery4 = -11.6;
float       angle1   =  0.0, angle2   =  0.0, angle3   =   0.0, angle4   =   0.0;
long        hueShift =  0;
/*******************************************/

/*************** Night Mode ****************/
struct TimerObject{
  int hour, minute;
};

TimerObject clock_on  = { 7, 0};  //daytime mode start time
TimerObject clock_off = {20, 0};  //night time mode start time
/*******************************************/

int Power_Mode = 1;  

/************ PROTOTYPES **************/
int setMode(string command);
void quickWeather();
void getWeather();
void processWeather(const char *name, const char *data);
void showWeather();
void drawWeatherIcon(uint8_t x, uint8_t y, int id);
void scrollBigMessage(char *m);
void scrollMessage(char* top, char* bottom ,uint8_t top_font_size,uint8_t bottom_font_size, uint16_t top_color, uint16_t bottom_color);
void pacClear();
void pacMan();
void drawPac(int x, int y, int z);
void drawGhost( int x, int y, int color);
void drawScaredGhost( int x, int y);
void cls();
void pong();
unsigned short pong_get_ball_endpoint(float tempballpos_x, float  tempballpos_y, float  tempballvel_x, float tempballvel_y);
void normal_clock();
void vectorNumber(int n, int x, int y, Color color, float scale_x, float scale_y);
void word_clock();
void jumble();
void display_date();
void flashing_cursor(unsigned short xpos, unsigned short ypos, unsigned short cursor_width, unsigned short cursor_height, unsigned short repeats);
void drawString(int x, int y, char* c,uint8_t font_size, uint16_t color);
void drawString(int x, int y, char* c,uint8_t font_size, Color color);
void drawChar(Canvas *canvas, int x, int y, char c, uint8_t font_size, Color color);
void drawChar(int x, int y, char c, uint8_t font_size, uint16_t color);
int calc_font_displacement(uint8_t font_size);
void spectrumDisplay();
void plasma();
void marquee();
void nitelite();
int timerEvaluate(const struct TimerObject on_time, const struct TimerObject off_time, const unsigned int currentTime);
time_t tmConvert_t(int YYYY, unsigned short MM, unsigned short DD, unsigned short hh, unsigned short mm, unsigned short ss);
#ifdef DST_CENTRAL_EUROPE
  bool IsDst(int day, int month, int dayOfWeek);
#endif

#ifdef DST_NORTH_AMERICA
  bool IsDST(int dayOfMonth, int month, int dayOfWeek);
#endif
void setup();
void loop();


/*************************************/
//RGBMatrix matrix;
Canvas *canvas;
Font mFont;

volatile bool interrupt_received = false;

static void InterruptHandler(int signo) {
  interrupt_received = true;
  canvas->Clear();
  delete canvas;
 
  exit(0);
}


int main(int argc, char *argv[]) {

  DEBUG("In main\n");
  RGBMatrix::Options defaults;
  defaults.hardware_mapping = "adafruit-hat";  // or e.g. "adafruit-hat"
  defaults.rows = 32;
  defaults.chain_length = 1;
  defaults.parallel = 1;
  defaults.show_refresh_rate = false;
  canvas = rgb_matrix::CreateMatrixFromFlags(&argc, &argv, &defaults);
  if (canvas == NULL)
    return 1;

/*
  rgb_matrix::Font font;
  if (!font.LoadFont(bdf_font_file)) {
    fprintf(stderr, "Couldn't load font '%s'\n", bdf_font_file);
    return usage(argv[0]);
  }
*/

  // It is always good to set up a signal handler to cleanly exit when we
  // receive a CTRL-C for instance. The DrawOnCanvas() routine is looking
  // for that.
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  setup();
  
  while(true) {
     loop();
     sleep(10);
  }

  // Animation finished. Shut down the RGB matrix.
//  led_matrix_delete(matrix);
  
  return 0;
}

void setup() {

        DEBUG("In Setup\n");

// TODO - sync RTC

//	matrix.begin();
	setTextWrap(canvas, false); // Allow text to run off right edge
	setTextSize(canvas, 1);
	setTextColor(canvas,Color(210, 210, 210));

#if defined useFFT
	memset(peak, 0, sizeof(peak));
	memset(col , 0, sizeof(col));

	for(uint8_t i=0; i<32; i++) {
		minLvlAvg[i] = 0;
		maxLvlAvg[i] = 255;
	}
#endif

//	randomSeed(analogRead(A7));

        DEBUG("Clearing Screen\n");
	// Clear Matrix
        canvas->Clear();
	
	DEBUG("Calling Pacman\n");
	pacMan();
	DEBUG("Calling quick weather\n");
	quickWeather();

	clock_mode = random(0,MAX_CLOCK_MODE-1);
	modeSwitch = millis();
	badWeatherCall = 0;			// counts number of unsuccessful webhook calls, reset after 3 failed calls
	updateCTime = millis();		// Reset 24hr cloud time refresh counter
	DEBUG("Exiting Setup\n");
}


void loop()
{
  DEBUG("In loop\n");
	
  int Power_Mode = timerEvaluate(clock_on, clock_off, Time.now());  // comment out to skip night time mode

  if (Power_Mode == 1)
  {
    DEBUG("Power Mode is 1\n");
    // !!!  Add code for re-syncing time every 24 hrs  !!!
    if ((millis() - updateCTime) > (24UL * 60UL * 60UL * 1000UL)) {
//      Spark.syncTime();
      updateCTime = millis();
    }

    if (millis() - modeSwitch > 120000UL) {	//Switch modes every 5 mins
      clock_mode++;
      mode_changed = 1;
      modeSwitch = millis();
      if (clock_mode > MAX_CLOCK_MODE - 1)
        clock_mode = 0;
      DEBUG("Switch mode to \n");
      DEBUGpln(clock_mode);
    }

    //reset clock type clock_mode
    switch (clock_mode) {
      case 0:
        normal_clock();
        break;
      case 1:
        pong();
        break;
      case 2:
        word_clock();
        break;
      case 3:
        jumble();
        break;
      case 4:
        spectrumDisplay();
        break;
      case 5:
        plasma();
        break;
      case 6:
        marquee();
        break;
      default:
        normal_clock();
        break;
    }

    //if the mode hasn't changed, show the date
    pacClear();
    if (mode_changed == 0) {
      display_date();
      pacClear();
    }
    else {
      //the mode has changed, so don't bother showing the date, just go to the new mode.
      mode_changed = 0; //reset mdoe flag.
    }
  }
  else
  {
    DEBUG("Power Mode is NOT 1\n");
    if(mode_changed == 1)
    {
	cls();
//TODO add back
//      matrix.swapBuffers(false);
      mode_changed = 0;
    }
    nitelite();
  }
  DEBUG("Leaving loop\n");
}

int setMode(string command)
{
/*
	int p = 0;
	while (p<(int)command.length()) {
		int i = command.indexOf(',',p);
		if (i<0) i = command.length();
		
		int j = command.indexOf('=',p);
		if (j<0) return -1;
		
		string key = command.substring(p,j);
		string value = command.substring(j+1,i);
		int val = value.toInt();
		
		// global params
		if(key == "normal")
		{
			mode_changed = 1;
			clock_mode = 0;
		}
		else if(key == "pong")
		{
			mode_changed = 1;
			clock_mode = 1;
		}
		else if(key == "word")
		{
			mode_changed = 1;
			clock_mode = 2;
		}
		else if(key == "jumble")
		{
			mode_changed = 1;
			clock_mode = 3;
		}
		else if(key == "spectrum")
		{
			mode_changed = 1;
			clock_mode = 4;
		}
		else if(key == "quick")
		{
			mode_quick = true;
			return 1;
		}
		else if(key == "plasma")
		{
			mode_changed = 1;
			clock_mode = 5;
		}
		else if(key == "marquee")
		{
			mode_changed = 1;
			clock_mode = 6;
		}
		else if(key == "city")
		{
			unsigned char tmp[20] = "";
			int p = value.length();
//command.getBytes(value, 0, j+1);
			command.copy(value, 0, p);
			strcpy(city, "{\"mycity\": \"");
			strcat(city, (const char *)tmp);
			strcat(city, "\" }");
			weatherGood = false;
			return 1;
		}
		if (mode_changed == 1) {
			modeSwitch = millis();
			return 1;
		}	  
		else return -1;

		p = i+1;
		}
	return 1;
}
*/	
	
	mode_changed = 0;

	int j = command.find_first_of('=',0);
	if (j>0) {	// "=" is used when setting city only
		if(command.substr(0,j) == "city")
		{
			char tmp[20] = "";
			int p = command.length();
			command.copy(tmp, (p-j), j+1);
			strcpy(city, "\"mycity\": \"");
			strcat(city, (const char *)tmp);
			strcat(city, "\" ");
			weatherGood = false;
			return 1;
		}		
	}
	else if(command == "normal")
	{
		mode_changed = 1;
		clock_mode = 0;
	}
	else if(command == "pong")
	{
		mode_changed = 1;
		clock_mode = 1;
	}
	else if(command == "word")
	{
		mode_changed = 1;
		clock_mode = 2;
	}
	else if(command == "jumble")
	{
		mode_changed = 1;
		clock_mode = 3;
	}
	else if(command == "spectrum")
	{
		mode_changed = 1;
		clock_mode = 4;
	}
	else if(command == "quick")
	{
		mode_quick = true;
		return 1;
	}
	else if(command == "plasma")
	{
		mode_changed = 1;
		clock_mode = 5;
	}
	else if(command == "marquee")
	{
		mode_changed = 1;
		clock_mode = 6;
	}
	if (mode_changed == 1) {
		modeSwitch = millis();
		return 1;
	}	  
	else return -1;
	
}


//*****************Weather Stuff*********************

void quickWeather(){
	getWeather();
	if(weatherGood){
		showWeather();
		//*** If city has changed, then since weatherGood then store city in EEPROM ***
	}
	else{
		cls();
		drawPixel(canvas,0,0, Color(1,0,0));
		drawPixel(canvas, 0,0,Color(1,0,0));
//		matrix.swapBuffers(true);
		DEBUG("Sleep\n");
		sleep(10);
	}
	DEBUG("exiting quickWeather\n");
}

void getWeather(){
	DEBUGpln("in getWeather");
	char vars[90];
/*	
	strcpy(vars, "{");
	strcat(vars, city);
	strcat(vars, ",");
	strcat(vars, UNITS);
	strcat(vars, ",");
	strcat(vars, API_KEY);
	strcat(vars, "}");
	
	weatherGood = false;
	// publish the event with city data that will trigger the webhook
	//Spark.publish(HOOK_PUB, city, 60, PRIVATE);
//	Spark.publish(HOOK_PUB, vars, 60, PRIVATE);

	unsigned long wait = millis();
//	while(!weatherGood && (millis() < wait + 5000UL))	//wait for subscribe to kick in or 5 secs
//		Spark.process();

	if (!weatherGood) {
		DEBUGn("Weather update failed\n");
		badWeatherCall++;
		if (badWeatherCall > 4)		//If 3 webhook call fail in a row, do a system reset
			System.reset();
	}
	else
		badWeatherCall = 0;
*/
}

void processWeather(const char *name, const char *data){
	weatherGood = true;
	lastWeatherTime = millis();
	stringPos = strlen((const char *)data);
	DEBUGpln("in process weather");

	memset(&w_temp,0,8*7);
	memset(&w_id,0,8*4);
	int dayCounter =0;
	int itemCounter = 0;
	int tempStringLoc=0;
	bool dropChar(false);
	
	for (int i=0; i<stringPos; i++){
		if(data[i]=='~'){
			itemCounter++;
			tempStringLoc = 0;
			dropChar = false;
			if(itemCounter>1){
				dayCounter++;
				itemCounter=0;
			}
		}
		else if(data[i]=='.' || data[i]=='"'){
			//if we get a . we want to drop all characters until the next ~
			dropChar=true;
		}
		else{
			if(!dropChar){
				switch(itemCounter){
				case 0:
					w_temp[dayCounter][tempStringLoc++] = data[i];
					break;
				case 1:
					w_id[dayCounter][tempStringLoc++] = data[i];
					break;
				}
			}
		}
	}
}

void showWeather(){
	unsigned short dow = Time.weekday()-1;
	char daynames[7][4]={
		"Sun", "Mon","Tue", "Wed", "Thu", "Fri", "Sat"
	};
	DEBUGpln("in showWeather");
	for(int i = 0 ; i<7; i++){

		int numTemp = atoi(w_temp[i]);
		//fix within range to generate colour value
		if (numTemp<-14) numTemp=-10;
		if (numTemp>34) numTemp =30;
		//add 14 so it falls between 0 and 48
		numTemp = numTemp +14;
		//divide by 3 so value between 0 and 16
		numTemp = numTemp / 3;

//		int tempColor;
		Color tempColor(0,0,0);
		if(numTemp<8){
			
			tempColor.r = 0;
			tempColor.g = numTemp/2;
			tempColor.b = 7;
//			tempColor = matrix.Color444(0,tempColor/2,7);
		}
		else{
			tempColor.r = 7;
			tempColor.g = (7-numTemp/2);
			tempColor.b = 0;
//			tempColor = matrix.Color444(7,(7-numTemp/2) ,0); 
		} 

		cls();

		//Display the day on the top line.
		if(i==0){
			drawString(2,2,(char*)"Now",51,Color(1,1,1));
//			drawString(2,2,(char*)"Now",51,matrix.Color444(1,1,1));
		}
		else{
//			drawString(2,2,daynames[(dow+i-1) % 7],51,matrix.Color444(0,1,0));
			drawString(2,2,daynames[(dow+i-1) % 7],51,Color(0,1,0));
			DEBUGpln(daynames[(dow+i-1)%7]);
		}

		//put the temp underneath
		bool positive = !(w_temp[i][0]=='-');
		for(int t=0; t<7; t++){
			if(w_temp[i][t]=='-'){
//				matrix.drawLine(3,10,4,10,tempColor);
				DrawLine(canvas, 3,10,4,10,tempColor);
			}
			else if(!(w_temp[i][t]==0)){
				vectorNumber(w_temp[i][t]-'0',t*4+2+(positive*2),8,tempColor,1,1);
			}
		}

// TODO
//		matrix.swapBuffers(true);
		drawWeatherIcon(16,0,atoi(w_id[i]));

//		Spark.process();	//Give the background process some lovin'

	}
}

void drawWeatherIcon(uint8_t x, uint8_t y, int id){
	unsigned long start = millis();
	static int rain[12];
	
	for(int r=0; r<13; r++){
		//rain[r]=random(9,18);
		rain[r]=random(9,15);
	}
	Color rainColor = Color(0,0,1);
	unsigned short intensity=id-(id/10)*10 + 1;

	int deep =0;
	bool raining(false);
	DEBUGpln(id);
	DEBUGpln(intensity);

	while(millis()<start+5000){
		switch(id/100){
		case 2:
			//Thunder
//			matrix.fillRect(x,y,16,16,matrix.Color333(0,0,0));
//			matrix.drawBitmap(x,y,cloud_outline,16,16,matrix.Color333(1,1,1));
			fillRect(canvas, x,y,16,16,Color(0,0,0));
			drawBitmap(canvas, x,y,cloud_outline,16,16,Color(1,1,1));
			if(random(0,10)==3){
				int pos = random(-5,5);
				drawBitmap(canvas, pos+x,y,lightning,16,16,Color(1,1,1));
			}
			raining = true;
			break;
		case 3:  
			//drizzle
			fillRect(canvas, x,y,16,16,Color(0,0,0));
			drawBitmap(canvas, x,y,cloud,16,16,Color(1,1,1));
			raining=true;
			break;
		case 5:
			//rain was 5
			fillRect(canvas, x,y,16,16,Color(0,0,0));
			
			if(intensity<3){
				drawBitmap(canvas, x,y,cloud,16,16,Color(1,1,1));
			}
			else{
				drawBitmap(canvas, x,y,cloud_outline,16,16,Color(1,1,1));
			}
			raining = true;
			break;
		case 6:
			//snow was 6
			rainColor = Color(4,4,4);
			fillRect(canvas, x,y,16,16,Color(0,0,0));
			
			deep = (millis()-start)/500;
			if(deep>6) deep=6;

			if(intensity<3){
				drawBitmap(canvas, x,y,cloud,16,16,Color(1,1,1));
				fillRect(canvas, x,y+16-deep/2,16,deep/2,rainColor);
			}
			else{
				drawBitmap(canvas, x,y,cloud_outline,16,16,Color(1,1,1));
				fillRect(canvas, x,y+16-(deep),16,deep,rainColor);
			}
			raining = true;
			break;  
		case 7:
			//atmosphere
			drawRect(canvas, x,y,16,16,Color(1,0,0));
//			drawString(x+2,y+6,(char*)"FOG",51,matrix.Color(1,1,1));
			drawString(x+2,y+6,(char*)"FOG",51,Color(1,1,1));
			break;
		case 8:
			//cloud
			fillRect(canvas, x,y,16,16,Color(0,0,1));
			if(id==800){
				drawBitmap(canvas, x,y,big_sun,16,16,Color(2,2,0));
			}
			else{
				if(id==801){
					drawBitmap(canvas, x,y,big_sun,16,16,Color(2,2,0));
					drawBitmap(canvas, x,y,cloud,16,16,Color(1,1,1));
				}
				else{
					if(id==802 || id ==803){
						drawBitmap(canvas, x,y,small_sun,16,16,Color(1,1,0));
					}
					drawBitmap(canvas, x,y,cloud,16,16,Color(1,1,1));
					drawBitmap(canvas, x,y,cloud_outline,16,16,Color(0,0,0));
				}
			}
			break;
		case 9:
			//extreme
			fillRect(canvas, x,y,16,16,Color(0,0,0));
			drawRect(canvas, x,y,16,16,Color(7,0,0));
			if(id==906){
				raining =true; 
				intensity=3;
				drawBitmap(canvas, x,y,cloud,16,16,Color(1,1,1));
			};
			break;
		default:
			fillRect(canvas, x,y,16,16,Color(0,1,1));
			drawBitmap(canvas, x,y,big_sun,16,16,Color(2,2,0));
			break;    
		}
		if(raining){
			for(int r = 0; r<13; r++){
//				matrix.drawPixel(x+r+2, rain[r]++, rainColor);
				drawPixel(canvas, x+r+2, rain[r]++, rainColor);
				if(rain[r]==16) rain[r]=9;
				//if(rain[r]==20) rain[r]=9;
			}
		} 
//TODO		matrix.swapBuffers(false);
//		Spark.process();	//Give the background process some lovin'
		sleep(( 50 -( intensity * 10 )) < 0 ? 0: 50-intensity*10);
	}
}
//*****************End Weather Stuff*********************


void scrollBigMessage(char *m){

	setTextSize(canvas, 1);
	int l = (strlen(m)*-6) - 32;
	for(int i = 32; i > l; i--){
		cls();
		setCursor(canvas, i,1);
		setTextColor(canvas, Color(1,1,1));
// TODO		matrix.print(m);
//TODO		matrix.swapBuffers(false);
		sleep(50);
//		Spark.process();
	}

}
/*
void scrollMessage(char* top, char* bottom ,uint8_t top_font_size,uint8_t bottom_font_size, uint16_t top_color, uint16_t bottom_color){

	int l = ((strlen(top)>strlen(bottom)?strlen(top):strlen(bottom))*-5) - 32;
	
	for(int i=32; i > l; i--){
		
		if (mode_changed == 1 || mode_quick)
			return;

		cls();
		
//		drawString(i,1,top,top_font_size, top_color);
		drawString(i,1,top,top_font_size, top_color);
//		drawString(i,9,bottom, bottom_font_size, bottom_color);
		drawString(i,9,bottom, bottom_font_size, bottom_color);
		matrix.swapBuffers(false);
		sleep(50);
	}

}
*/

//Runs pacman or other animation, refreshes weather data
void pacClear(){
	DEBUG("in pacClear\n");
	//refresh weather if we havent had it for 30 mins
	//or the last time we had it, it was bad, 
	//or weve never had it before.
	if((millis()>lastWeatherTime+1800000) || lastWeatherTime==0 || !weatherGood) getWeather();

	if(!wasWeatherShownLast && weatherGood){
		showWeather();
		wasWeatherShownLast = true;
	}
	else{  
		wasWeatherShownLast = false;

		pacMan();
	}
}  


void pacMan(){
#if defined (usePACMAN)
	DEBUG("in pacMan\n");
	if(powerPillEaten>0){
		for(int i =32+(powerPillEaten*17); i>-17; i--){
			long nowish = millis();
			cls();

			drawPac(i,0,-1);
			if(powerPillEaten>0) drawScaredGhost(i-17,0);
			if(powerPillEaten>1) drawScaredGhost(i-34,0);
			if(powerPillEaten>2) drawScaredGhost(i-51,0);
			if(powerPillEaten>3) drawScaredGhost(i-68,0);

			matrix.swapBuffers(false);    
			while(millis()-nowish<50) 
//				Spark.process();	//Give the background process some lovin'
		}
		powerPillEaten = 0;
	}
	else{  

		int hasEaten = 0;

		int powerPill = random(0,5);
		int numGhosts=random(0,4);
		if(powerPill ==0){
			if(numGhosts==0) numGhosts++;
			powerPillEaten = numGhosts;
		}

		for(int i=-17; i<32+(numGhosts*17); i++){
			cls();
			long nowish = millis();
			for(int j = 0; j<6;j++){

				if( j*5> i){
					if(powerPill==0 && j==4){
						fillCircle(canvas, j*5,8,2,Color7,3,0));
					}
					else{
						fillRect(canvas, j*5,8,2,2,Color(7,3,0));
					}
				}
			}

			if(i==19 && powerPill == 0) hasEaten=1;
			drawPac(i,0,1);
			if(hasEaten == 0){
				if(numGhosts>0) drawGhost(i-17,0,Color(3,0,3));
				if(numGhosts>1) drawGhost(i-34,0,Color(3,0,0));
				if(numGhosts>2) drawGhost(i-51,0,Color(0,3,3));
				if(numGhosts>3) drawGhost(i-68,0,Color(7,3,0));
			}
			else{
				if(numGhosts>0) drawScaredGhost(i-17-(i-19)*2,0);
				if(numGhosts>1) drawScaredGhost(i-34-(i-19)*2,0);
				if(numGhosts>2) drawScaredGhost(i-51-(i-19)*2,0);
				if(numGhosts>3) drawScaredGhost(i-68-(i-19)*2,0);
			}
			matrix.swapBuffers(false);
			while(millis()-nowish<50) 
//				Spark.process();	//Give the background process some lovin'
		}
	}
#endif //usePACMAN
}

#if defined (usePACMAN)
void drawPac(int x, int y, int z){
	Color c = Color(3,3,0);
	if(x>-16 && x<32){
		if(abs(x)%4==0){
			drawBitmap(canvas, x,y,(z>0?pac:pac_left),16,16,c);
		}
		else if(abs(x)%4==1 || abs(x)%4==3){
			drawBitmap(canvas, x,y,(z>0?pac2:pac_left2),16,16,c);
		}
		else{
			drawBitmap(canvas, x,y,(z>0?pac3:pac_left3),16,16,c);
		}
	}
}

void drawGhost( int x, int y, Color color){
	if(x>-16 && x<32){
		if(abs(x)%8>3){
			drawBitmap(canvas, x,y,blinky,16,16,color);
		}
		else{
			drawBitmap(canvas, x,y,blinky2,16,16,color);
		}
		drawBitmap(canvas, x,y,eyes1,16,16,Color(3,3,3));
		drawBitmap(canvas, x,y,eyes2,16,16,Color(0,0,7));
	}
}  

void drawScaredGhost( int x, int y){
	if(x>-16 && x<32){
		if(abs(x)%8>3){
			drawBitmap(canvas, x,y,blinky,16,16,matrix.Color333(0,0,7));
		}
		else{
			drawBitmap(canvas, x,y,blinky2,16,16,Color(0,0,7));
		}
		drawBitmap(canvas, x,y,scared,16,16,Color(7,3,2));
	}
}  
#endif  //usePACMAN


void cls(){
//	matrix.fillScreen(0);
//        canvas->Fill(cMap[C_BLACK].r, cMap[C_BLACK].g, cMap[C_BLACK].b);
        canvas->Fill(0,0,0);
}

void pong(){
	DEBUGpln("in Pong");
	setTextSize(canvas, 1);
	setTextColor(canvas, Color(2, 2, 2));

	float ballpos_x, ballpos_y;
	float ballvel_x, ballvel_y;
	int bat1_y = 5;  //bat starting y positions
	int bat2_y = 5;  
	int bat1_target_y = 5;  //bat targets for bats to move to
	int bat2_target_y = 5;
	unsigned short bat1_update = 1;  //flags - set to update bat position
	unsigned short bat2_update = 1;
	unsigned short bat1miss, bat2miss; //flags set on the minute or hour that trigger the bats to miss the ball, thus upping the score to match the time.
	unsigned short restart = 1;   //game restart flag - set to 1 initially to setup 1st game

	cls();

//	for(int i=0; i< SHOWCLOCK; i++) {
	int showTime = Time.now();
	
	while((Time.now() - showTime) < showClock) {
		cls();
		//draw pitch centre line
		int adjust = 0;
		if(Time.second()%2==0)adjust=1;
		for (unsigned short i = 0; i <16; i++) {
			if ( i % 2 == 0 ) { //plot point if an even number
//				matrix.drawPixel(16,i+adjust,matrix.Color333(0,4,0));
				drawPixel(canvas, 16,i+adjust,Color(0,4,0));
			}
		} 

		//main pong game loop
		if (mode_changed == 1)
		return;
		if(mode_quick){
			mode_quick = false;
			display_date();
			quickWeather();
			pong();
			return;
		}	

		int ampm=0;
		//update score / time
		unsigned short mins = Time.minute();
		unsigned short hours = Time.hour();
		if (hours > 12) {
			hours = hours - ampm * 12;
		}
		if (hours < 1) {
			hours = hours + ampm * 12;
		}

		char buffer[3];

		itoa(hours,buffer,10);
		//fix - as otherwise if num has leading zero, e.g. "03" hours, itoa coverts this to chars with space "3 ". 
		if (hours < 10) {
			buffer[1] = buffer[0];
			buffer[0] = '0';
		}
		vectorNumber(buffer[0]-'0',8,1,Color(1,1,1),1,1);
		vectorNumber(buffer[1]-'0',12,1,Color(1,1,1),1,1);

		itoa(mins,buffer,10); 
		if (mins < 10) {
			buffer[1] = buffer[0];
			buffer[0] = '0';
		} 
		vectorNumber(buffer[0]-'0',18,1,Color(1,1,1),1,1);
		vectorNumber(buffer[1]-'0',22,1,Color(1,1,1),1,1);

		//if restart flag is 1, setup a new game
		if (restart) {
			//set ball start pos
			ballpos_x = 16;
			ballpos_y = random (4,12);

			//pick random ball direction
			if (random(0,2) > 0) {
				ballvel_x = 1; 
			} 
			else {
				ballvel_x = -1;
			}
			if (random(0,2) > 0) {
				ballvel_y = 0.5; 
			} 
			else {
				ballvel_y = -0.5;
			}
			//draw bats in initial positions
			bat1miss = 0; 
			bat2miss = 0;
			//reset game restart flag
			restart = 0;
		}

		//if coming up to the minute: secs = 59 and mins < 59, flag bat 2 (right side) to miss the return so we inc the minutes score
		if (Time.second() == 59 && Time.minute() < 59){
			bat1miss = 1;
		}
		// if coming up to the hour: secs = 59  and mins = 59, flag bat 1 (left side) to miss the return, so we inc the hours score.
		if (Time.second() == 59 && Time.minute() == 59){
			bat2miss = 1;
		}

		//AI - we run 2 sets of 'AI' for each bat to work out where to go to hit the ball back 
		//very basic AI...
		// For each bat, First just tell the bat to move to the height of the ball when we get to a random location.
		//for bat1
		if (ballpos_x == random(18,32)){
			bat1_target_y = ballpos_y;
		}
		//for bat2
		if (ballpos_x == random(4,16)){
			bat2_target_y = ballpos_y;
		}

		//when the ball is closer to the left bat, run the ball maths to find out where the ball will land
		if (ballpos_x == 15 && ballvel_x < 0) {

			unsigned short end_ball_y = pong_get_ball_endpoint(ballpos_x, ballpos_y, ballvel_x, ballvel_y);

			//if the miss flag is set,  then the bat needs to miss the ball when it gets to end_ball_y
			if (bat1miss == 1){
				bat1miss = 0;
				if ( end_ball_y > 8){
					bat1_target_y = random (0,3); 
				} 
				else {
					bat1_target_y = 8 + random (0,3);              
				}      
			} 
			//if the miss flag isn't set,  set bat target to ball end point with some randomness so its not always hitting top of bat
			else {
				bat1_target_y = end_ball_y - random (0, 6);        
				//check not less than 0
				if (bat1_target_y < 0){
					bat1_target_y = 0;
				}
				if (bat1_target_y > 10){
					bat1_target_y = 10;
				} 
			}
		}

		//right bat AI
		//if positive velocity then predict for right bat - first just match ball height
		//when the ball is closer to the right bat, run the ball maths to find out where it will land
		if (ballpos_x == 17 && ballvel_x > 0) {

			unsigned short end_ball_y = pong_get_ball_endpoint(ballpos_x, ballpos_y, ballvel_x, ballvel_y);

			//if flag set to miss, move bat out way of ball
			if (bat2miss == 1){
				bat2miss = 0;
				//if ball end point above 8 then move bat down, else move it up- so either way it misses
				if (end_ball_y > 8){
					bat2_target_y = random (0,3); 
				} 
				else {
					bat2_target_y = 8 + random (0,3);
				}      
			} 
			else {
				//set bat target to ball end point with some randomness 
				bat2_target_y =  end_ball_y - random (0,6);
				//ensure target between 0 and 15
				if (bat2_target_y < 0){
					bat2_target_y = 0;
				} 
				if (bat2_target_y > 10){
					bat2_target_y = 10;
				} 
			}
		}

		//move bat 1 towards target    
		//if bat y greater than target y move down until hit 0 (dont go any further or bat will move off screen)
		if (bat1_y > bat1_target_y && bat1_y > 0 ) {
			bat1_y--;
			bat1_update = 1;
		}

		//if bat y less than target y move up until hit 10 (as bat is 6)
		if (bat1_y < bat1_target_y && bat1_y < 10) {
			bat1_y++;
			bat1_update = 1;
		}

		//draw bat 1
		if (bat1_update){
			fillRect(canvas, BAT1_X-1,bat1_y,2,6,Color(0,0,4));
		}

		//move bat 2 towards target (dont go any further or bat will move off screen)
		//if bat y greater than target y move down until hit 0
		if (bat2_y > bat2_target_y && bat2_y > 0 ) {
			bat2_y--;
			bat2_update = 1;
		}

		//if bat y less than target y move up until hit max of 10 (as bat is 6)
		if (bat2_y < bat2_target_y && bat2_y < 10) {
			bat2_y++;
			bat2_update = 1;
		}

		//draw bat2
		if (bat2_update){
			fillRect(canvas, BAT2_X+1,bat2_y,2,6,Color(0,0,4));
		}

		//update the ball position using the velocity
		ballpos_x =  ballpos_x + ballvel_x;
		ballpos_y =  ballpos_y + ballvel_y;

		//check ball collision with top and bottom of screen and reverse the y velocity if either is hit
		if (ballpos_y <= 0 ){
			ballvel_y = ballvel_y * -1;
			ballpos_y = 0; //make sure value goes no less that 0
		}

		if (ballpos_y >= 15){
			ballvel_y = ballvel_y * -1;
			ballpos_y = 15; //make sure value goes no more than 15
		}

		//check for ball collision with bat1. check ballx is same as batx
		//and also check if bally lies within width of bat i.e. baty to baty + 6. We can use the exp if(a < b && b < c) 
		if ((int)ballpos_x == BAT1_X+1 && (bat1_y <= (int)ballpos_y && (int)ballpos_y <= bat1_y + 5) ) { 

			//random if bat flicks ball to return it - and therefor changes ball velocity
			if(!random(0,3)) { //not true = no flick - just straight rebound and no change to ball y vel
				ballvel_x = ballvel_x * -1;
			} 
			else {
				bat1_update = 1;
				unsigned short flick;  //0 = up, 1 = down.

				if (bat1_y > 1 || bat1_y < 8){
					flick = random(0,2);   //pick a random dir to flick - up or down
				}

				//if bat 1 or 2 away from top only flick down
				if (bat1_y <=1 ){
					flick = 0;   //move bat down 1 or 2 pixels 
				} 
				//if bat 1 or 2 away from bottom only flick up
				if (bat1_y >=  8 ){
					flick = 1;  //move bat up 1 or 2 pixels 
				}

				switch (flick) {
					//flick up
				case 0:
					bat1_target_y = bat1_target_y + random(1,3);
					ballvel_x = ballvel_x * -1;
					if (ballvel_y < 2) {
						ballvel_y = ballvel_y + 0.2;
					}
					break;

					//flick down
				case 1:   
					bat1_target_y = bat1_target_y - random(1,3);
					ballvel_x = ballvel_x * -1;
					if (ballvel_y > 0.2) {
						ballvel_y = ballvel_y - 0.2;
					}
					break;
				}
			}
		}

		//check for ball collision with bat2. check ballx is same as batx
		//and also check if bally lies within width of bat i.e. baty to baty + 6. We can use the exp if(a < b && b < c) 
		if ((int)ballpos_x == BAT2_X && (bat2_y <= (int)ballpos_y && (int)ballpos_y <= bat2_y + 5) ) { 

			//random if bat flicks ball to return it - and therefor changes ball velocity
			if(!random(0,3)) {
				ballvel_x = ballvel_x * -1;    //not true = no flick - just straight rebound and no change to ball y vel
			} 
			else {
				bat1_update = 1;
				unsigned short flick;  //0 = up, 1 = down.

				if (bat2_y > 1 || bat2_y < 8){
					flick = random(0,2);   //pick a random dir to flick - up or down
				}
				//if bat 1 or 2 away from top only flick down
				if (bat2_y <= 1 ){
					flick = 0;  //move bat up 1 or 2 pixels 
				} 
				//if bat 1 or 2 away from bottom only flick up
				if (bat2_y >=  8 ){
					flick = 1;   //move bat down 1 or 2 pixels 
				}

				switch (flick) {
					//flick up
				case 0:
					bat2_target_y = bat2_target_y + random(1,3);
					ballvel_x = ballvel_x * -1;
					if (ballvel_y < 2) {
						ballvel_y = ballvel_y + 0.2;
					}
					break;

					//flick down
				case 1:   
					bat2_target_y = bat2_target_y - random(1,3);
					ballvel_x = ballvel_x * -1;
					if (ballvel_y > 0.2) {
						ballvel_y = ballvel_y - 0.2;
					}
					break;
				}
			}
		}

		//plot the ball on the screen
		unsigned short plot_x = (int)(ballpos_x + 0.5f);
		unsigned short plot_y = (int)(ballpos_y + 0.5f);

//		matrix.drawPixel(plot_x,plot_y,matrix.Color333(4, 0, 0));
		drawPixel(canvas, plot_x,plot_y,Color(4, 0, 0));

		//check if a bat missed the ball. if it did, reset the game.
		if ((int)ballpos_x == 0 ||(int) ballpos_x == 32){
			restart = 1; 
		}

//		Spark.process();	//Give the background process some lovin'
		sleep(40);
//TODO		matrix.swapBuffers(false);
	} 
}
unsigned short pong_get_ball_endpoint(float tempballpos_x, float  tempballpos_y, float  tempballvel_x, float tempballvel_y) {

	//run prediction until ball hits bat
	while (tempballpos_x > BAT1_X && tempballpos_x < BAT2_X  ){
		tempballpos_x = tempballpos_x + tempballvel_x;
		tempballpos_y = tempballpos_y + tempballvel_y;
		//check for collisions with top / bottom
		if (tempballpos_y <= 0 || tempballpos_y >= 15){
			tempballvel_y = tempballvel_y * -1;
		}    
	}  
	return tempballpos_y; 
}

void normal_clock()
{
	DEBUGpln("in normal_clock");
	setTextWrap(canvas, false); // Allow text to run off right edge
	setTextSize(canvas, 2);
	setTextColor(canvas, Color(2, 3, 2));

	cls();
	unsigned short hours = Time.hour();
	unsigned short mins = Time.minute();

	int  msHourPosition = 0;
	int  lsHourPosition = 0;
	int  msMinPosition = 0;
	int  lsMinPosition = 0;      
	int  msLastHourPosition = 0;
	int  lsLastHourPosition = 0;
	int  msLastMinPosition = 0;
	int  lsLastMinPosition = 0;      

	//Start with all characters off screen
	int c1 = -17;
	int c2 = -17;
	int c3 = -17;
	int c4 = -17;

	float scale_x =2.5;
	float scale_y =3.0;


	char lastHourBuffer[3]="  ";
	char lastMinBuffer[3] ="  ";

	//loop to display the clock for a set duration of SHOWCLOCK
	//for (int show = 0; show < SHOWCLOCK ; show++) {
	int showTime = Time.now();
	
	while((Time.now() - showTime) < showClock) {
		cls();

		if (mode_changed == 1)
		return;
		if(mode_quick){
			mode_quick = false;
			display_date();
			quickWeather();
			normal_clock();
			return;
		}

		//udate mins and hours with the new time
		mins = Time.minute();
		hours = Time.hour();

		char buffer[3];

		itoa(hours,buffer,10);
		//fix - as otherwise if num has leading zero, e.g. "03" hours, itoa coverts this to chars with space "3 ". 
		if (hours < 10) {
			buffer[1] = buffer[0];
			buffer[0] = '0';
		}

		if(lastHourBuffer[0]!=buffer[0] && c1==0) c1= -17;
		if( c1 < 0 )c1++;
		msHourPosition = c1;
		msLastHourPosition = c1 + 17;

		if(lastHourBuffer[1]!=buffer[1] && c2==0) c2= -17;
		if( c2 < 0 )c2++;
		lsHourPosition = c2;
		lsLastHourPosition = c2 + 17;

		//update the display
		//shadows first
		vectorNumber((lastHourBuffer[0]-'0'), 2, 2+msLastHourPosition, Color(0,0,1),scale_x,scale_y);
		vectorNumber((lastHourBuffer[1]-'0'), 9, 2+lsLastHourPosition, Color(0,0,1),scale_x,scale_y);
		vectorNumber((buffer[0]-'0'), 2, 2+msHourPosition, Color(0,0,1),scale_x,scale_y);
		vectorNumber((buffer[1]-'0'), 9, 2+lsHourPosition, Color(0,0,1),scale_x,scale_y); 

		vectorNumber((lastHourBuffer[0]-'0'), 1, 1+msLastHourPosition, Color(1,1,1),scale_x,scale_y);
		vectorNumber((lastHourBuffer[1]-'0'), 8, 1+lsLastHourPosition, Color(1,1,1),scale_x,scale_y);
		vectorNumber((buffer[0]-'0'), 1, 1+msHourPosition, Color(1,1,1),scale_x,scale_y);
		vectorNumber((buffer[1]-'0'), 8, 1+lsHourPosition, Color(1,1,1),scale_x,scale_y);    

		if(c1==0) lastHourBuffer[0]=buffer[0];
		if(c2==0) lastHourBuffer[1]=buffer[1];

		fillRect(canvas, 16,5,2,2,Color(0,0,Time.second()%2));
		fillRect(canvas, 16,11,2,2,Color(0,0,Time.second()%2));

		fillRect(canvas, 15,4,2,2,Color(Time.second()%2,Time.second()%2,Time.second()%2));
		fillRect(canvas, 15,10,2,2,Color(Time.second()%2,Time.second()%2,Time.second()%2));

		itoa (mins, buffer, 10);
		if (mins < 10) {
			buffer[1] = buffer[0];
			buffer[0] = '0';
		}

		if(lastMinBuffer[0]!=buffer[0] && c3==0) c3= -17;
		if( c3 < 0 )c3++;
		msMinPosition = c3;
		msLastMinPosition= c3 + 17;

		if(lastMinBuffer[1]!=buffer[1] && c4==0) c4= -17;
		if( c4 < 0 )c4++;
		lsMinPosition = c4;
		lsLastMinPosition = c4 + 17;

		vectorNumber((buffer[0]-'0'), 19, 2+msMinPosition, Color(0,0,1),scale_x,scale_y);
		vectorNumber((buffer[1]-'0'), 26, 2+lsMinPosition, Color(0,0,1),scale_x,scale_y);
		vectorNumber((lastMinBuffer[0]-'0'), 19, 2+msLastMinPosition, Color(0,0,1),scale_x,scale_y);
		vectorNumber((lastMinBuffer[1]-'0'), 26, 2+lsLastMinPosition, Color(0,0,1),scale_x,scale_y);

		vectorNumber((buffer[0]-'0'), 18, 1+msMinPosition, Color(1,1,1),scale_x,scale_y);
		vectorNumber((buffer[1]-'0'), 25, 1+lsMinPosition, Color(1,1,1),scale_x,scale_y);
		vectorNumber((lastMinBuffer[0]-'0'), 18, 1+msLastMinPosition, Color(1,1,1),scale_x,scale_y);
		vectorNumber((lastMinBuffer[1]-'0'), 25, 1+lsLastMinPosition, Color(1,1,1),scale_x,scale_y);

		if(c3==0) lastMinBuffer[0]=buffer[0];
		if(c4==0) lastMinBuffer[1]=buffer[1];

//TODO		swapBuffers(false); 
//		Spark.process();	//Give the background process some lovin'
	}
}

//Draw number n, with x,y as top left corner, in chosen color, scaled in x and y.
//when scale_x, scale_y = 1 then character is 3x5
void vectorNumber(int n, int x, int y, Color color, float scale_x, float scale_y){

	switch (n){
	case 0:
		DrawLine(canvas, x ,y , x , y+(4*scale_y) , color);
		DrawLine(canvas, x , y+(4*scale_y) , x+(2*scale_x) , y+(4*scale_y), color);
		DrawLine(canvas, x+(2*scale_x) , y , x+(2*scale_x) , y+(4*scale_y) , color);
		DrawLine(canvas, x ,y , x+(2*scale_x) , y , color);
		break; 
	case 1: 
		DrawLine(canvas,  x+(1*scale_x), y, x+(1*scale_x),y+(4*scale_y), color);  
		DrawLine(canvas, x , y+4*scale_y , x+2*scale_x , y+4*scale_y,color);
		DrawLine(canvas, x,y+scale_y, x+scale_x, y,color);
		break;
	case 2:
		DrawLine(canvas, x ,y , x+2*scale_x , y , color);
		DrawLine(canvas, x+2*scale_x , y , x+2*scale_x , y+2*scale_y , color);
		DrawLine(canvas, x+2*scale_x , y+2*scale_y , x , y+2*scale_y, color);
		DrawLine(canvas, x , y+2*scale_y, x , y+4*scale_y,color);
		DrawLine(canvas, x , y+4*scale_y , x+2*scale_x , y+4*scale_y,color);
		break; 
	case 3:
		DrawLine(canvas, x ,y , x+2*scale_x , y , color);
		DrawLine(canvas, x+2*scale_x , y , x+2*scale_x , y+4*scale_y , color);
		DrawLine(canvas, x+2*scale_x , y+2*scale_y , x+scale_x , y+2*scale_y, color);
		DrawLine(canvas, x , y+4*scale_y , x+2*scale_x , y+4*scale_y,color);
		break;
	case 4:
		DrawLine(canvas, x+2*scale_x , y , x+2*scale_x , y+4*scale_y , color);
		DrawLine(canvas, x+2*scale_x , y+2*scale_y , x , y+2*scale_y, color);
		DrawLine(canvas, x ,y , x , y+2*scale_y , color);
		break;
	case 5:
		DrawLine(canvas, x ,y , x+2*scale_x , y , color);
		DrawLine(canvas, x , y , x , y+2*scale_y , color);
		DrawLine(canvas, x+2*scale_x , y+2*scale_y , x , y+2*scale_y, color);
		DrawLine(canvas, x+2*scale_x , y+2*scale_y, x+2*scale_x , y+4*scale_y,color);
		DrawLine(canvas,  x , y+4*scale_y , x+2*scale_x , y+4*scale_y,color);
		break; 
	case 6:
		DrawLine(canvas, x ,y , x , y+(4*scale_y) , color);
		DrawLine(canvas, x ,y , x+2*scale_x , y , color);
		DrawLine(canvas, x+2*scale_x , y+2*scale_y , x , y+2*scale_y, color);
		DrawLine(canvas, x+2*scale_x , y+2*scale_y, x+2*scale_x , y+4*scale_y,color);
		DrawLine(canvas, x+2*scale_x , y+4*scale_y , x, y+(4*scale_y) , color);
		break;
	case 7:
		DrawLine(canvas, x ,y , x+2*scale_x , y , color);
		DrawLine(canvas,  x+2*scale_x, y, x+scale_x,y+(4*scale_y), color);
		break;
	case 8:
		DrawLine(canvas, x ,y , x , y+(4*scale_y) , color);
		DrawLine(canvas, x , y+(4*scale_y) , x+(2*scale_x) , y+(4*scale_y), color);
		DrawLine(canvas, x+(2*scale_x) , y , x+(2*scale_x) , y+(4*scale_y) , color);
		DrawLine(canvas, x ,y , x+(2*scale_x) , y , color);
		DrawLine(canvas, x+2*scale_x , y+2*scale_y , x , y+2*scale_y, color);
		break;
	case 9:
		DrawLine(canvas, x ,y , x , y+(2*scale_y) , color);
		DrawLine(canvas, x , y+(4*scale_y) , x+(2*scale_x) , y+(4*scale_y), color);
		DrawLine(canvas, x+(2*scale_x) , y , x+(2*scale_x) , y+(4*scale_y) , color);
		DrawLine(canvas, x ,y , x+(2*scale_x) , y , color);
		DrawLine(canvas, x+2*scale_x , y+2*scale_y , x , y+2*scale_y, color);
		break;    
	}
}




void display_date()
{
	DEBUGpln("in display_date");
	Color color = Color(0,1,0);
	cls();
//TODO	matrix.swapBuffers(true);
	//read the date from the DS1307
	//it returns the month number, day number, and a number representing the day of week - 1 for Tue, 2 for Wed 3 for Thu etc.
	unsigned short dow = Time.weekday()-1;		//we  take one off the value the DS1307 generates, as our array of days is 0-6 and the DS1307 outputs  1-7.
	unsigned short date = Time.day();
	unsigned short mont = Time.month()-1; 

	//array of day and month names to print on the display. Some are shortened as we only have 8 characters across to play with 
	char daynames[7][9]={
		"Sunday", "Monday","Tuesday", "Wed", "Thursday", "Friday", "Saturday"                  };
	char monthnames[12][9]={
		"January", "February", "March", "April", "May", "June", "July", "August", "Sept", "October", "November", "December"                  };

	//call the flashing cursor effect for one blink at x,y pos 0,0, height 5, width 7, repeats 1
	flashing_cursor(0,0,3,5,1);

	//print the day name
	int i = 0;
	while(daynames[dow][i])
	{
		flashing_cursor(i*4,0,3,5,0);
		drawChar(canvas, i*4,0,daynames[dow][i],51,color);
//TODO		matrix.swapBuffers(true);
		i++;

		if (mode_changed == 1)
		return;
	}

	//pause at the end of the line with a flashing cursor if there is space to print it.
	//if there is no space left, dont print the cursor, just wait.
	if (i*4 < 32){
		flashing_cursor(i*4,0,3,5,1);  
	} 
	else {
//		Spark.process();	//Give the background process some lovin'
		sleep(300);
	}

	//flash the cursor on the next line  
	flashing_cursor(0,8,3,5,0);

	//print the date on the next line: First convert the date number to chars
	char buffer[3];
	itoa(date,buffer,10);

	//then work out date 2 letter suffix - eg st, nd, rd, th etc
	char suffix[4][3]={
		"st", "nd", "rd", "th"                    };
	unsigned short s = 3; 
	if(date == 1 || date == 21 || date == 31) {
		s = 0;
	} 
	else if (date == 2 || date == 22) {
		s = 1;
	} 
	else if (date == 3 || date == 23) {
		s = 2;
	} 

	//print the 1st date number
	drawChar(canvas, 0,8,buffer[0],51,color);
//TODO	matrix.swapBuffers(true);

	//if date is under 10 - then we only have 1 digit so set positions of sufix etc one character nearer
	unsigned short suffixposx = 4;

	//if date over 9 then print second number and set xpos of suffix to be 1 char further away
	if (date > 9){
		suffixposx = 8;
		flashing_cursor(4,8,3,5,0); 
		drawChar(canvas, 4,8,buffer[1],51,color);
//TODO		matrix.swapBuffers(true);
	}

	//print the 2 suffix characters
	flashing_cursor(suffixposx, 8,3,5,0);
	drawChar(canvas, suffixposx,8,suffix[s][0],51,color);
//TODO	matrix.swapBuffers(true);

	flashing_cursor(suffixposx+4,8,3,5,0);
	drawChar(canvas, suffixposx+4,8,suffix[s][1],51,color);
//TODO	matrix.swapBuffers(true);

	//blink cursor after 
	flashing_cursor(suffixposx + 8,8,3,5,1);  

	//replace day name with date on top line - effectively scroll the bottom line up by 8 pixels
	for(int q = 8; q>=0; q--){
		cls();
		int w =0 ;
		while(daynames[dow][w])
		{
			drawChar(canvas, w*4,q-8,daynames[dow][w],51,color);

			w++;
		}

//TODO		matrix.swapBuffers(true);
		//date first digit
		drawChar(canvas, 0,q,buffer[0],51,color);
		//date second digit - this may be blank and overwritten if the date is a single number
		drawChar(canvas, 4,q,buffer[1],51,color);
		//date suffix
		drawChar(canvas, suffixposx,q,suffix[s][0],51,color);
		//date suffix
		drawChar(canvas, suffixposx+4,q,suffix[s][1],51,color);
//TODO		matrix.swapBuffers(true);
		sleep(50);
	}
	//flash the cursor for a second for effect
	flashing_cursor(suffixposx + 8,0,3,5,0);  

	//print the month name on the bottom row
	i = 0;
	while(monthnames[mont][i])
	{  
		flashing_cursor(i*4,8,3,5,0);
		drawChar(canvas, i*4,8,monthnames[mont][i],51,color);
//TODO		matrix.swapBuffers(true);
		i++; 

	}

	//blink the cursor at end if enough space after the month name, otherwise juts wait a while
	if (i*4 < 32){
		flashing_cursor(i*4,8,3,5,2);  
	} 
	else {
		sleep(1000);
	}

	for(int q = 8; q>=-8; q--){
		cls();
		int w =0 ;
		while(monthnames[mont][w])
		{
			drawChar(canvas, w*4,q,monthnames[mont][w],51,color);

			w++;
		}

//TODO		matrix.swapBuffers(true);
		//date first digit
		drawChar(canvas, 0,q-8,buffer[0],51,color);
		//date second digit - this may be blank and overwritten if the date is a single number
		drawChar(canvas, 4,q-8,buffer[1],51,color);
		//date suffix
		drawChar(canvas, suffixposx,q-8,suffix[s][0],51,color);
		//date suffix
		drawChar(canvas, suffixposx+4,q-8,suffix[s][1],51,color);
//TODO		matrix.swapBuffers(true);
		sleep(50);
	}
}


/*
* flashing_cursor
* print a flashing_cursor at xpos, ypos and flash it repeats times 
*/
void flashing_cursor(unsigned short xpos, unsigned short ypos, unsigned short cursor_width, unsigned short cursor_height, unsigned short repeats)
{
	for (unsigned short r = 0; r <= repeats; r++) {
		fillRect(canvas, xpos,ypos,cursor_width, cursor_height, Color(0,3,0));
//TODO		matrix.swapBuffers(true);

		if (repeats > 0) {
			sleep(400);
		} 
		else {
			sleep(70);
		}

		fillRect(canvas, xpos,ypos,cursor_width, cursor_height, Color(0,0,0));
//TODO		matrix.swapBuffers(true);

		//if cursor set to repeat, wait a while
		if (repeats > 0) {
			sleep(400); 
		}
//		Spark.process();	//Give the background process some lovin'
	}
}
void drawString(int x, int y, char* c,uint8_t font_size, Color color)
{
// TODO make font reflect size

        // x & y are positions, c-> pointer to string to disp, update_s: false(write to mem), true: write to disp
        //font_size : 51(ascii value for 3), 53(5) and 56(8)
	DrawText(canvas, mFont, 2,2, color, NULL, c);
}


void drawString(int x, int y, char* c,uint8_t font_size, uint16_t color)
{
	// x & y are positions, c-> pointer to string to disp, update_s: false(write to mem), true: write to disp
	//font_size : 51(ascii value for 3), 53(5) and 56(8)
	for(uint16_t i=0; i< strlen(c); i++)
	{
//		drawChar(canvas, x, y, c[i],font_size, color);
		x+=calc_font_displacement(font_size); // Width of each glyph
	}
}

int calc_font_displacement(uint8_t font_size)
{
	switch(font_size)
	{
	case 51:
		return 4;  //5x3 hence occupies 4 columns ( 3 + 1(space btw two characters))
		break;

	case 53:
		return 6;
		break;
		//case 56:
		//return 6;
		//break;
	default:
		return 6;
		break;
	}
}

void drawChar(Canvas *canvas, int x, int y, char c, uint8_t font_size, Color color)  // Display the data depending on the font size mentioned in the font_size variable
{
 int i;
 i = mFont.DrawGlyph(canvas, x, y, color, c);
}

void drawChar(int x, int y, char c, uint8_t font_size, uint16_t color)  // Display the data depending on the font size mentioned in the font_size variable
{
/*
	uint8_t dots;
	if ((c >= 'A' && c <= 'Z') ||
			(c >= 'a' && c <= 'z')) {
		c &= 0x1F;   // A-Z maps to 1-26
	} 
	else if (c >= '0' && c <= '9') {
		c = (c - '0') + 27;
	} 
	else if (c == ' ') {
		c = 0; // space
	}
	else if (c == '#'){
		c=37;
	}
	else if (c=='/'){
		c=37;
	}

	switch(font_size)
	{
	case 51:  // font size 3x5  ascii value of 3: 51

		if(c==':'){
//			matrix.drawPixel(x+1,y+1,color);
//			matrix.drawPixel(x+1,y+3,color);
			drawPixel(canvas,x+1,y+1,color);
			drawPixel(canvas,x+1,y+3,color);
		}
		else if(c=='-'){
			matrix.drawLine(x,y+2,3,0,color);
		}
		else if(c=='.'){
			matrix.drawPixel(x+1,y+2,color);
		}
		else if(c==39 || c==44){
			matrix.drawLine(x+1,y,2,0,color);
			matrix.drawPixel(x+2,y+1,color);
		}
		else{
			for (uint8_t row=0; row< 5; row++) {
				dots = font3x5[(uint8_t)c][row];
				for (uint8_t col=0; col < 3; col++) {
					int x1=x;
					int y1=y;
					if (dots & (4>>col))
					matrix.drawPixel(x1+col, y1+row, color);
				}    
			}
		}
		break;

	case 53:  // font size 5x5   ascii value of 5: 53

		if(c==':'){
			matrix.drawPixel(x+2,y+1,color);
			matrix.drawPixel(x+2,y+3,color);
		}
		else if(c=='-'){
			matrix.drawLine(x+1,y+2,3,0,color);
		}
		else if(c=='.'){
			matrix.drawPixel(x+2,y+2,color);
		}
		else if(c==39 || c==44){
			matrix.drawLine(x+2,y,2,0,color);
			matrix.drawPixel(x+4,y+1,color);
		}
		else{
			for (uint8_t row=0; row< 5; row++) {
				dots = font5x5[(uint8_t)c][row];
				for (uint8_t col=0; col < 5; col++) {
					int x1=x;
					int y1=y;
					if (dots & (64>>col))  // For some wierd reason I have the 5x5 font in such a way that.. last two bits are zero.. 
					matrix.drawPixel(x1+col, y1+row, color);        
				}
			}
		}          

		break;
	default:
		break;
	}
*/
}



int timerEvaluate(const struct TimerObject on_time, const struct TimerObject off_time, const unsigned int currentTime)
{
  unsigned int start_time = tmConvert_t(Time.year(currentTime), Time.month(currentTime), Time.day(currentTime), on_time.hour, on_time.minute, 0);
  unsigned int end_time = tmConvert_t(Time.year(currentTime), Time.month(currentTime), Time.day(currentTime), off_time.hour, off_time.minute, 0);

  if (start_time < end_time)
  {
    if (start_time < currentTime && currentTime < end_time) return 1;
    else return 0;
  }
  else if (start_time > end_time)
  {
    if (currentTime > start_time || currentTime < end_time) return 1;
    else return 0;
  }
  else return 0;
}

inline time_t tmConvert_t(int YYYY, unsigned short MM, unsigned short DD, unsigned short hh, unsigned short mm, unsigned short ss)  // inlined for speed
{
  struct tm t;
  t.tm_year = YYYY-1900;
  t.tm_mon = MM - 1;
  t.tm_mday = DD;
  t.tm_hour = hh;
  t.tm_min = mm;
  t.tm_sec = ss;
  t.tm_isdst = 0;  // not used
  time_t t_of_day = mktime(&t);
  return t_of_day;
}

// North American Daylight Savings

#ifdef DST_NORTH_AMERICA
bool IsDST(int dayOfMonth, int month, int dayOfWeek)
{
  if (month < 3 || month > 11)
  {
    return false;
  }
  if (month > 3 && month < 11)
  {
    return true;
  }
  int previousSunday = dayOfMonth - (dayOfWeek - 1);
  if (month == 3)
  {
    return previousSunday >= 8;
  }
  return previousSunday <= 0;
}
#endif

#ifdef DST_CENTRAL_EUROPE
bool IsDst(int day, int month, int dayOfWeek)
{
  if (month < 3 || month > 10)
  {
    return false;
  }
  if (month > 3 && month < 10)
  {
    return true;
  }
  int previousSunday = day - dayOfWeek;
  if (month == 3)
  {
    return previousSunday >= 25;
  }
  if (month == 10) return previousSunday < 25;
  {
    return false;
  }
}

#endif

//TODO
void word_clock() {}

void nitelite() {

  DEBUG("In nitelite\n");

  static int lastSecond = 60;
  int nowTime = Time.now();
  int nowHour = Time.hour(nowTime);
  nowHour %= 12;
  if (nowHour == 0) nowHour = 12;
  int nowMinute = Time.minute(nowTime);
  int nowSecond = Time.second(nowTime);
  if(lastSecond != nowSecond)
  {
    DEBUG("Doing nite stuff\n");
    cls();
    char nowBuffer[5] = "";
    sprintf(nowBuffer, "%2d%02d", nowHour, nowMinute);
    fillCircle(canvas, 7, 6, 7, Color(10, 10, 10)); // moon
    fillCircle(canvas, 9, 4, 7, Color(0, 0, 0));    // cutout the crescent
    drawPixel(canvas, 16, 3, Color(10, 10, 10));    // stars
    drawPixel(canvas, 30, 2, Color(10, 10, 10));
    drawPixel(canvas, 19, 6, Color(10, 10, 10));
    drawPixel(canvas, 21, 1, Color(10, 10, 10));
    vectorNumber(nowBuffer[0] - '0', 15, 11, Color(10, 10, 10), 1, 1);
    vectorNumber(nowBuffer[1] - '0', 19, 11, Color(10, 10, 10), 1, 1);
    vectorNumber(nowBuffer[2] - '0', 25, 11, Color(10, 10, 10), 1, 1);
    vectorNumber(nowBuffer[3] - '0', 29, 11, Color(10, 10, 10), 1, 1);
    drawPixel(canvas, 23, 12, (nowSecond % 2)? Color(5, 5, 5) : Color(0, 0, 0));
    drawPixel(canvas, 23, 14, (nowSecond % 2)? Color(5, 5, 5) : Color(0, 0, 0));
//TODO    matrix.swapBuffers(false);
  }
  lastSecond = nowSecond;
  DEBUG("Leaving nite stuff\n");
}

void marquee() {}
void jumble() {}
void plasma() {}
void spectrumDisplay() {}

