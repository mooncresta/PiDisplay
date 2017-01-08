
#include "led-matrix.h" // Hardware-specific library
#include "graphics.h"
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


/*************************************/
//RGBMatrix matrix;
Canvas *canvas;
Font mFont;

volatile bool interrupt_received = false;

static void InterruptHandler(int signo) {
  interrupt_received = true;
}

void testcalls() {

  printf("Starting test pattern\n");

  canvas->Clear();
//  canvas->Fill(0, 0, 255);
  
  printf("Night test\n");

    char nowBuffer[5] = "";
    sprintf(nowBuffer, "%2d%02d", 10, 10);
    fillCircle(canvas, 7, 6, 7, Color(10, 10, 10)); // moon
    fillCircle(canvas, 9, 4, 7, Color(0, 0, 0));    // cutout the crescent
    drawPixel(canvas, 16, 3, Color(10, 10, 10));    // stars
    drawPixel(canvas, 30, 2, Color(10, 10, 10));
    drawPixel(canvas, 19, 6, Color(10, 10, 10));
    drawPixel(canvas, 21, 1, Color(10, 10, 10));
//    vectorNumber(nowBuffer[0] - '0', 15, 11, Color(10, 10, 10), 1, 1);
//    vectorNumber(nowBuffer[1] - '0', 19, 11, Color(10, 10, 10), 1, 1);
//    vectorNumber(nowBuffer[2] - '0', 25, 11, Color(10, 10, 10), 1, 1);
//    vectorNumber(nowBuffer[3] - '0', 29, 11, Color(10, 10, 10), 1, 1);
//    drawPixel(canvas, 23, 12, (10 % 2)? Color(5, 5, 5) : Color(0, 0, 0));
    drawPixel(canvas, 23, 14, (10 % 2)? Color(5, 5, 5) : Color(0, 0, 0));

  sleep(10000);
  printf("End Night test\n");

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
    usleep(1 * 1000);  // wait a little to slow down things.
  }
  printf("Ending test pattern\n");
}


int main(int argc, char *argv[]) {

  RGBMatrix::Options defaults;
  defaults.hardware_mapping = "adafruit-hat";  // or e.g. "adafruit-hat"
  defaults.rows = 32;
  defaults.chain_length = 1;
  defaults.parallel = 1;
  defaults.show_refresh_rate = true;
  canvas = rgb_matrix::CreateMatrixFromFlags(&argc, &argv, &defaults);
  if (canvas == NULL)
    return 1;

  // It is always good to set up a signal handler to cleanly exit when we
  // receive a CTRL-C for instance. The DrawOnCanvas() routine is looking
  // for that.
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  testcalls();

  canvas->Clear();
  delete canvas;
}

