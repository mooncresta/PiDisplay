#include "PiDisplay.h"
//#include "led-matrix.h" // Hardware-specific library
//#include "graphics.h"
#include <iostream>
#include "string.h"
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "bridge.h"
#include "trng.h"

#include "weather_symbols.h"

using namespace rgb_matrix;
using namespace std;


/*************************************/
// GLOBALS

RGBMatrix *matrix;
FrameCanvas *canvas;
Font mFont;

TimeClass Time;

int stringPos;
int mode_changed = 0;                   // Flag if mode changed.
bool mode_quick = false;                // Quick weather display
int clock_mode = 0;                             // Default clock mode (1 = pong)
uint16_t showClock = 300;               // Default time to show a clock face
unsigned long modeSwitch;
unsigned long updateCTime;              // 24hr timer for resyncing cloud time


volatile bool interrupt_received = false;

static void InterruptHandler(int signo) {
  interrupt_received = true;
  canvas->Clear();
  delete matrix;
  DEBUG("EXITING .........\n");

  exit(0);

}


void nite()
{  
    printf("Night test\n");
    char nowBuffer[5] = "";
    sprintf(nowBuffer, "%2d%02d", 10, 10);
    fillCircle(7, 6, 7, Color(10, 10, 10)); // moon
    fillCircle(9, 4, 7, Color(0, 0, 0));    // cutout the crescent
    drawPixel( 16, 3, Color(10, 10, 10));    // stars
    drawPixel(30, 2, Color(10, 10, 10));
    drawPixel(19, 6, Color(10, 10, 10));
    drawPixel( 21, 1, Color(10, 10, 10));
    vectorNumber(nowBuffer[0] - '0', 15, 11, Color(10, 10, 10), 1, 1);
    vectorNumber(nowBuffer[1] - '0', 19, 11, Color(10, 10, 10), 1, 1);
    vectorNumber(nowBuffer[2] - '0', 25, 11, Color(10, 10, 10), 1, 1);
    vectorNumber(nowBuffer[3] - '0', 29, 11, Color(10, 10, 10), 1, 1);
    drawPixel(23, 12, (10 % 2)? Color(5, 5, 5) : Color(0, 0, 0));
    drawPixel(23, 14, (10 % 2)? Color(5, 5, 5) : Color(0, 0, 0));
   swapBuffers(false);

  printf("End Night test\n");
}

void p0()
{
   printf("Primtives\n");

   printf("Cloud\n");
   cls();
   drawBitmap(0,0,cloud_outline, 16, 16, Color(100,100,100));
   swapBuffers(false);
   sleep(2);

   printf("Rectangle\n");
   cls();
   drawRect(0,0,16, 16, Color(20,20,20));
   swapBuffers(false);
   sleep(2);

   printf("Fill Rectangle\n");
   fillRect(0,0,16, 16, Color(255,255,0));
   swapBuffers(false);
   sleep(2);

   printf("Draw Circle\n");
   cls();
   drawCircle(16,16, 16, Color(0,0,255));
   swapBuffers(false);
   sleep(2);

   printf("Fill Circle\n");
   cls();
   fillCircle(16,16, 16, Color(0,0,255));
   swapBuffers(false);
   sleep(2);
  
   printf("Draw String\n");
   cls();
   drawString(1,1,"Text 112", 51, Color(0,255,0));
/*
   char text[] = "Hello There";
   fprintf(stderr, "Length is '%d'\n", strlen(text));
        rgb_matrix::Font font;
        if (!font.LoadFont("./fonts/5x7.bdf")) {
              printf("Couldn't load font\n");
              return;
        }
  fprintf(stderr, "Font height is '%d'\n", font.height());
  if (canvas == NULL)
    return;

printf("Before DrawText\n");
   canvas->SetBrightness(75);
   rgb_matrix::DrawText(canvas, font, 1,1 + font.baseline(), Color(255,255,0), text);
*/
   swapBuffers(false);
   sleep(5);
   
   printf("Scroll Big Message\n");
   cls();
   scrollBigMessage("Hello There");
   swapBuffers(false);
   sleep(5);

   printf("Scroll Message\n");
   cls();
   scrollMessage("Top Line Message", "Bottom Line Special", 53, 53, Color(255,0,0), Color(0,255,0));
   swapBuffers(false);
   sleep(5);
}

void p1()
{
  printf("P1 test\n");
  int center_x = canvas->width() / 2;
  int center_y = canvas->height() / 2;
  float radius_max = canvas->width() / 2;
  float angle_step = 1.0 / 360;
  for (float a = 0, r = 0; r < radius_max; a += angle_step, r += angle_step) {
    if (interrupt_received)
      return;
    float dot_x = cos(a * 2 * M_PI) * r;
    float dot_y = sin(a * 2 * M_PI) * r;
    canvas->SetPixel(center_x + dot_x, center_y + dot_y,
                     255, 0, 0);
    swapBuffers(false);
    usleep(1 * 1000);  // wait a little to slow down things.
  }


  printf("Ending test pattern\n");
}

void testcalls() {

  printf("Starting test pattern\n");
  canvas->Clear();
  p0();
  sleep(10);
  p1();
  sleep(10);
}

int main(int argc, char *argv[]) {

  RGBMatrix::Options defaults;
  defaults.hardware_mapping = "adafruit-hat";  // or e.g. "adafruit-hat"
  defaults.rows = 32;
  defaults.chain_length = 1;
  defaults.parallel = 1;
  defaults.brightness = 100;
  defaults.show_refresh_rate = true;

   matrix = rgb_matrix::CreateMatrixFromFlags(&argc, &argv, &defaults);
   if (matrix == NULL)
    return 1;

  // Drawing Canvas
  canvas = matrix->CreateFrameCanvas();
  if (canvas == NULL)
    return 1;


  // It is always good to set up a signal handler to cleanly exit when we
  // receive a CTRL-C for instance. The DrawOnCanvas() routine is looking
  // for that.
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  testcalls();

  canvas->Clear();
  delete matrix;
}

