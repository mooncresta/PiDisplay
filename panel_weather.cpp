#include "PiDisplay.h"

#ifdef PANEL_WEATHER

#include "weather_symbols.h"

int stringPos;

// Function prototypes
void quickWeather();
void getWeather();
void processWeather(const char *name, const char *data);
void showWeather();
void drawWeatherIcon(uint8_t x, uint8_t y, int id);
void drawDayOfWeek();


/***** Weather webhook definitions *****/
//#define HOOK_RESP		"hook-response/rgbpc_weather_hook"
//#define HOOK_PUB		"rgbpc_weather_hook"
/***************************************/


bool weatherGood = false;
int badWeatherCall = 0;
char w_temp[8][7] = {""};
char w_id[8][4] = {""};

/*** Set default city for weather webhook ***/
char city[40] = WEATHER_CITY;

boolean wasWeatherShownLast = true;
unsigned long lastWeatherTime = 0;

uint32_t weather_expire = 15 * 60 * 1000;
uint32_t weather_refresh = 0;

bool weather_hook_has_published = false;
uint32_t weather_hook_retry = 0;
uint32_t display_weather_icon = 0;


static char daynames[7][4] = {
	"Sun", "Mon","Tue", "Wed", "Thu", "Fri", "Sat"
};

uint8_t weather_dow = 0;

//*****************Weather Stuff*********************
void clearWeather() {
// Clear weather panel
   fillRect(K_W_X, K_WY-1, K_W_WIDTH, K_W_HEIGHT, Color(0,0,0));

};

void quickWeather(){
	getWeather();
	if(weatherGood){
		showWeather();
		//*** If city has changed, then since weatherGood then store city in EEPROM ***
	}
	else{
		clearWeather();
		drawPixel(canvas,0,0, Color(1,0,0));
		drawPixel(canvas, 0,0,Color(1,0,0));
		swapBuffers(true);
		DEBUG("Sleep\n");
		sleep(10);
	}
	DEBUG("exiting quickWeather\n");
}

void getWeather(){
	DEBUGpln("in getWeather");
	char vars[90];
	strcpy(vars, "{");
	strcat(vars, city);
	strcat(vars, ",");
	strcat(vars, UNITS);
	strcat(vars, ",");
	strcat(vars, API_KEY);
	strcat(vars, "}");

	weatherGood = false;
	// publish the event with city data that will trigger the webhook
//	Spark.publish(HOOK_PUB, vars, 60, PRIVATE);

	unsigned long wait = millis();
//	while(!weatherGood && (millis() < wait + 5000UL))	//wait for subscribe to kick in or 5 secs
//		Spark.process();

	if (!weatherGood) {
		DEBUGn("Weather update failed\n");
		badWeatherCall++;
		if (badWeatherCall > 4)		//If 3 webhook call fail in a row, do a system reset
			return;
	}
	else
		badWeatherCall = 0;
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
			drawString(K_W_X+2,K_W_Y+2,(char*)"Now",51,Color(1,1,1));
//			drawString(2,2,(char*)"Now",51,matrix.Color444(1,1,1));
		}
		else{
//			drawString(K_W_X+2,K_W_Y+2,daynames[(dow+i-1) % 7],51,matrix.Color444(0,1,0));
			drawString(2,2,daynames[(dow+i-1) % 7],51,Color(0,1,0));
			DEBUGpln(daynames[(dow+i-1)%7]);
		}

		//put the temp underneath
		bool positive = !(w_temp[i][0]=='-');
		for(int t=0; t<7; t++){
			if(w_temp[i][t]=='-'){
				DrawLine(canvas, K_W_X+3,K_W_Y+10,K_W_X+4,K_W_Y+10,tempColor);
			}
			else if(!(w_temp[i][t]==0)){
				vectorNumber(w_temp[i][t]-'0',t*4+2+(positive*2),8,tempColor,1,1);
			}
		}

		drawWeatherIcon(K_W_X+16,K_W_Y+0,atoi(w_id[i]));
		swapBuffers(true);

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
#endif
