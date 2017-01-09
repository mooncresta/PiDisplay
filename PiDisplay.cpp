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

// allow us to use itoa() in this scope
extern char* itoa(int a, char* buffer, unsigned char radix);

#define	PiDisplayversion	"V1.0"

#define DEBUGME
#define DEBUG(message) printf(message)
#define DEBUGp(message) printf(message)
#define DEBUGpln(message)

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


/*************** Night Mode ****************/
struct TimerObject{
  int hour, minute;
};

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
Canvas *canvas;
Font mFont;

volatile bool interrupt_received = false;

// Handle interrupt and Clean up
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


//TODO
void word_clock() {}

void marquee() {}
void jumble() {}
void plasma() {}
void spectrumDisplay() {}
