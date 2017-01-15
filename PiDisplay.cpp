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

#include "PiDisplay.h"

#define	PiDisplayversion	"V1.0"

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

/********* FONT STUFF ********/

// Font directory - assumes it's as per standard library name
#define FONT_DIR "./rpi-rgb-led-matrix/fonts/"  

#define NUM_FONTS 3   // To match below
t_FontLib FontLib[] = {
	{"4x6.bdf", 4, 6, NULL},  // SMALL	
	{"5x7.bdf", 5, 7, NULL},  // MEDIUM
	{"9x15.bdf", 0, 0, NULL}  // LARGE
};

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

int stringPos;

int mode_changed = 0;			// Flag if mode changed.
bool mode_quick = false;		// Quick weather display
int clock_mode = 0;				// Default clock mode (1 = pong)
uint16_t showClock = 300;		// Default time to show a clock face
unsigned long modeSwitch;
unsigned long updateCTime;		// 24hr timer for resyncing cloud time
boolean wasWeatherShownLast= true;
unsigned long lastWeatherTime =0;
boolean weatherGood=false;
int badWeatherCall;


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

////// Set when on and off /////
TimerObject clock_on  = { 7, 0};  //daytime mode start time
TimerObject clock_off = {20, 0};  //night time mode start time
/*******************************************/

int Power_Mode = 1;



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
// GLOBALS
RGBMatrix *matrix;  // Active matrix
FrameCanvas *canvas; // Active Canvas
Font mFont;

volatile bool interrupt_received = false;

// Handle interrupt and Clean up
static void InterruptHandler(int signo) {
  interrupt_received = true;
  canvas->Clear();
  delete matrix;

  DEBUG("EXITING .........\n");

  exit(0);
}


int main(int argc, char *argv[]) {

  DEBUG("In main\n");
  RGBMatrix::Options defaults;
  defaults.hardware_mapping = "adafruit-hat";  // or e.g. "adafruit-hat"
  defaults.rows = 32;
  defaults.chain_length = 2;
  defaults.parallel = 1;
  defaults.show_refresh_rate = false;
  matrix = rgb_matrix::CreateMatrixFromFlags(&argc, &argv, &defaults);
  if (matrix == NULL)
    return 1;

  // Drawing Canvas
  canvas = matrix->CreateFrameCanvas();
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

	setTextWrap(false); // Allow text to run off right edge
	setTextSize(1);
	setTextColor(Color(210, 210, 210));

   	// Setup Fonts
	char fontname[30];
	for(int i=0; i<NUM_FONTS; i++) {
	    FontLib[i].ptr = new Font;
	    sprintf(fontname, "%s%s", FONT_DIR, FontLib[i].name); 
	    if (!FontLib[i].ptr->LoadFont(fontname)) {
              printf("Couldn't load font\n");
	      return;
            }
	}
	
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
  canvas->Clear();

#ifdef PANEL_PACMAN
	DEBUG("Calling Pacman\n");
	pacMan();
#endif

#ifdef PANEL_WEATHER
	DEBUG("Calling quick weather\n");
	quickWeather();
	badWeatherCall = 0;			// counts number of unsuccessful webhook calls, reset after 3 failed calls
#endif

	clock_mode = random(0,MAX_CLOCK_MODE-1);
	modeSwitch = millis();
	updateCTime = millis();		// Reset 24hr cloud time refresh counter

#ifdef PANEL_MESSAGE
	MessageScroller *msgbar = new MessageScroller(matrix, 10, 10);
#endif
	DEBUG("Exiting Setup\n");
}

// Main Loop faked from Arduino
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
#ifdef PANEL_PONG
        pong();
#endif
        break;
      case 2:
#ifdef PANEL_WORDCLOCK
        word_clock();
#endif
        break;
      case 3:
#ifdef PANEL_JUMBLE
        jumble();
#endif
        break;
      case 4:
#ifdef PANEL_SPECTRUM
        spectrumDisplay();
#endif
        break;
      case 5:
#ifdef PANEL_PLASMA
        plasma();
#endif
        break;
      case 6:
#ifdef PANEL_MARQUEE
        marquee();
#endif
        break;
      default:
        normal_clock();
        break;
    }

    //if the mode hasn't changed, show the date
#ifdef PANEL_PACMAN
    pacClear();
#endif
    if (mode_changed == 0) {
      display_date();
#ifdef PANEL_PACMAN
      pacClear();
#endif
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
        swapBuffers(false);
        mode_changed = 0;
    }
    nitelite();
  }
  DEBUG("Leaving loop\n");
}

int setMode(string command)
{
    DEBUG("In setMode\n");
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
*/
	return 1;
}

/*
	mode_changed = 0;
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
*/

//TODO
/*
void word_clock() {}

void marquee() {}
void jumble() {}
void plasma() {}
void spectrumDisplay() {}
*/
