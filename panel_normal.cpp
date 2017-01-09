#include "PiDisplay.h"

void normal_clock()
{
	DEBUG("in normal_clock\n");
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
