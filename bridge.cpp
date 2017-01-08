#include "bridge.h"
//#include "led-matrix.h" // Hardware-specific library
//#include "graphics.h"
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

using namespace rgb_matrix;
using namespace std;

extern char* itoa(int num, char* str, unsigned char radix) {
    snprintf(str, sizeof(str), "%d", num);
}
namespace rgb_matrix {
// Adafruit GFX Functions
void drawBitmap(Canvas *canvas, int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, Color color)
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

void drawFastVLine(Canvas *canvas, int16_t x, int16_t y, int16_t h, Color color) 
{
  // Update in subclasses if desired!
  DrawLine(canvas, x, y, x, y+h-1, color);
}

void drawFastHLine(Canvas *canvas, int16_t x, int16_t y, int16_t w, Color color) 
{
  // Update in subclasses if desired!
  DrawLine(canvas, x, y, x+w-1, y, color);
}

// Draw a rectangle
void drawRect(Canvas *canvas, int16_t x, int16_t y, int16_t w, int16_t h, Color color)
{
  drawFastHLine(canvas, x, y, w, color);
  drawFastHLine(canvas, x, y+h-1, w, color);
  drawFastVLine(canvas, x, y, h, color);
  drawFastVLine(canvas, x+w-1, y, h, color);
}

void fillRect(Canvas *canvas, int16_t x, int16_t y, int16_t w, int16_t h,
			    Color color) {
  // Update in subclasses if desired!
  for (int16_t i=x; i<x+w; i++) {
    drawFastVLine(canvas, i, y, h, color);
  }
}

void setCursor(Canvas *canvas, int16_t x, int16_t y)
{
}

void setTextColor(Canvas *canvas, Color c)
{
}

void setTextWrap(Canvas *canvas, bool f)
{
}

void setTextSize(Canvas *canvas, int16_t s)
{
}

void drawPixel(Canvas *canvas, int16_t x, int16_t y, Color color)
{
    canvas->SetPixel(x,y, color.r,color.g, color.b);
}

void fillCircle(Canvas *canvas, int16_t x0, int16_t y0, int16_t r,
			      Color color) {
  drawFastVLine(canvas, x0, y0-r, 2*r+1, color);
  fillCircleHelper(canvas, x0, y0, r, 3, 0, color);
}

// Used to do circles and roundrects
void fillCircleHelper(Canvas *canvas, int16_t x0, int16_t y0, int16_t r,
    uint8_t cornername, int16_t delta, Color color) {

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
      drawFastVLine(canvas, x0+x, y0-y, 2*y+1+delta, color);
      drawFastVLine(canvas, x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) {
      drawFastVLine(canvas, x0-x, y0-y, 2*y+1+delta, color);
      drawFastVLine(canvas, x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}
} // End of namespace

int random(int low, int high)
{
    trng<unsigned int> rng_int;
    return(rng_int.random(low, high));
}


void initialiseEpoch()
{
  struct timeval tv ;

  gettimeofday (&tv, NULL) ;
  epochMilli = (uint64_t)tv.tv_sec * (uint64_t)1000    + (uint64_t)(tv.tv_usec / 1000) ;
  epochMicro = (uint64_t)tv.tv_sec * (uint64_t)1000000 + (uint64_t)(tv.tv_usec) ;
}

/*
 * millis:
 *	Return a number of milliseconds as an unsigned int.
 *********************************************************************************
 */

unsigned int millis (void)
{
  struct timeval tv ;
  uint64_t now ;

  gettimeofday (&tv, NULL) ;
  now  = (uint64_t)tv.tv_sec * (uint64_t)1000 + (uint64_t)(tv.tv_usec / 1000) ;

  return (uint32_t)(now - epochMilli) ;
}

/* the current weekday */
int TimeClass::weekday() {
	return (1);
}

/* the weekday for the given time */
int TimeClass::weekday(time_t t) {
	return (1);
}

time_t TimeClass::now() {
	time_t t;
	return (t);
}

int TimeClass::hour() {
        return (1);
}
int TimeClass::hour(time_t t) {
        return (1);
}

int TimeClass::second() {
        return (1);
}
int TimeClass::second(time_t t) {
        return (1);
}

int TimeClass::year() {
        return (1);
}
int TimeClass::year(time_t t) {
        return (1);
}

int TimeClass::minute() {
        return (1);
}
int TimeClass::minute(time_t t) {
        return (1);
}

int TimeClass::month() {
        return (1);
}
int TimeClass::month(time_t t) {
        return (1);
}

int TimeClass::day() {
        return (1);
}

int TimeClass::day(time_t t ) {
        return (1);
}

