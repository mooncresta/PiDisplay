#include "PiDisplay.h"
#include "font5x5.h"
#include "font3x5.h"

// Core Graphics functions

using namespace rgb_matrix;
using namespace std;

namespace rgb_matrix {


// Adafruit GFX Functions
void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, Color color)
{
  int16_t i, j, byteWidth = (w + 7) / 8;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(bitmap[j * byteWidth + i / 8] & (128 >> (i & 7))) {
	canvas->SetPixel(x+i, y+j, color.r, color.g, color.b);
      }
    }
  }
}

void swapBuffers(boolean copy) {
    // Move back canvas active
    canvas = matrix->SwapOnVSync(canvas);
}
void drawFastVLine(int16_t x, int16_t y, int16_t h, Color color)
{
  // Update in subclasses if desired!
  DrawLine(canvas, x, y, x, y+h-1, color);
}

void drawFastHLine(int16_t x, int16_t y, int16_t w, Color color)
{
  // Update in subclasses if desired!
  DrawLine(canvas, x, y, x+w-1, y, color);
}

// Draw a rectangle
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, Color color)
{
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y+h-1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x+w-1, y, h, color);
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
			    Color color) {
  // Update in subclasses if desired!
  for (int16_t i=x; i<x+w; i++) {
    drawFastVLine(i, y, h, color);
  }
}

// Clear Screen - Set to black
void cls(){
    canvas->Clear();
}



void setTextWrap(bool f)
{
//TODO	wrap = f;
}

void setTextSize(int s)
{
//TODO	 textsize = (s > 0) ? s : 1;
}

void drawPixel(int16_t x, int16_t y, Color color)
{
    canvas->SetPixel(x,y, color.r,color.g, color.b);
}

void drawCircle(int16_t x, int16_t y, int16_t r, Color color)
{
    DrawCircle(canvas, x,y, r, color);
}

void fillCircle(int16_t x0, int16_t y0, int16_t r,
			      Color color) {
  drawFastVLine(x0, y0-r, 2*r+1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}

// Used to do circles and roundrects
void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, Color color) {

  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1) {
      drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) {
      drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  } //endwhile
}

void  scrollBigMessage(char *m){
printf("In Scroll Big\n");

	setTextSize(1);
	int l = (strlen(m)*-6) - 32;
	for(int i = 32; i > l; i--){
		cls();
		setTextColor(Color(255,255,0));
		drawString(i,1, m, F_LARGE, Color(255,0,0));
		swapBuffers(false);

                usleep(3 * 10000);
        }
}


void scrollMessage(char* top, char* bottom ,uint8_t top_font_size,
		   uint8_t bottom_font_size, Color top_color, Color bottom_color){

	int l = ((strlen(top)>strlen(bottom)?strlen(top):strlen(bottom))*-5) - 32;

	for(int i=32; i > l; i--){
		if (mode_changed == 1 || mode_quick)
			return;
		cls();
		drawString(i,1,top,top_font_size, top_color);
		drawString(i,9,bottom, bottom_font_size, bottom_color);
		swapBuffers(false);
                usleep(3 * 10000);
	}

}


/*
* flashing_cursor
* print a flashing_cursor at xpos, ypos and flash it repeats times
*/
void flashing_cursor(unsigned short xpos, unsigned short ypos, unsigned short cursor_width, unsigned short cursor_height, unsigned short repeats)
{

// DOES NOT WORK !!!
	for (unsigned short r = 0; r <= repeats; r++) {
		fillRect(xpos,ypos,cursor_width, cursor_height, Color(0,3,0));
		swapBuffers(true);

		if (repeats > 0) {
			sleep(400);
		}
		else {
			sleep(70);
		}

		fillRect(xpos,ypos,cursor_width, cursor_height, Color(0,0,0));
		swapBuffers(true);

		//if cursor set to repeat, wait a while
		if (repeats > 0) {
			sleep(400);
		}
	}
}

void setCursor(int x, int y)
{
// DOES NOT WORK !!!
}

void setTextColor(Color c)
{
// DOES NOT WORK !!!
}


void drawString(int x, int y, char* c,uint8_t font_size, Color color)
{
// TODO make font reflect size

    int font_index = font_size; //TODO - better way to use size to select
    font_index = 0;
 
        // x & y are positions, c-> pointer to string to disp, update_s: false(write to mem), true: write to disp
        //font_size : 51(ascii value for 3), 53(5) and 56(8)
 /*
  * Load font. This needs to be a filename with a bdf bitmap font.
  */
       y = y+ FontLib[font_index].ptr->baseline();
	
// fprintf(stderr, "Font height is '%d'\n", FontLib[font_index].ptr->height());
// fprintf(stderr, "X is '%d'\n", x);
// fprintf(stderr, "Y is '%d'\n", y);

	rgb_matrix::DrawText(canvas, *FontLib[font_index].ptr, x,y, color, NULL, c);
}


void drawChar(int x, int y, char c, uint8_t font_size, Color color)  // Display the data depending on the font size mentioned in the font_size variable
{
   int i;
   i = mFont.DrawGlyph(canvas, x, y, color, c);
}


} // End of namespace

/*
void drawString(int x, int y, char* c,uint8_t font_size, uint16_t color)
{
	// x & y are positions, c-> pointer to string to disp, update_s: false(write to mem), true: write to disp
	//font_size : 51(ascii value for 3), 53(5) and 56(8)
	for(uint16_t i=0; i< strlen(c); i++)
	{
//TODO		drawChar(canvas, x, y, c[i],font_size, color);
		x+=calc_font_displacement(font_size); // Width of each glyph
	}
}
*/

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

// Display the data depending on the font size mentioned in the font_size variable
void drawChar(int x, int y, char c, uint8_t font_size, uint16_t color) {
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
			matrix.drawPixel(x+1,y+1,color);
			matrix.drawPixel(x+1,y+3,color);
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
