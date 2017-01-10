#include "PiDisplay.h"
using namespace rgb_matrix;

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
    fillCircle(7, 6, 7, Color(10, 10, 10)); // moon
    fillCircle( 9, 4, 7, Color(0, 0, 0));    // cutout the crescent
    drawPixel(16, 3, Color(10, 10, 10));    // stars
    drawPixel(30, 2, Color(10, 10, 10));
    drawPixel(19, 6, Color(10, 10, 10));
    drawPixel(21, 1, Color(10, 10, 10));
    vectorNumber(nowBuffer[0] - '0', 15, 11, Color(10, 10, 10), 1, 1);
    vectorNumber(nowBuffer[1] - '0', 19, 11, Color(10, 10, 10), 1, 1);
    vectorNumber(nowBuffer[2] - '0', 25, 11, Color(10, 10, 10), 1, 1);
    vectorNumber(nowBuffer[3] - '0', 29, 11, Color(10, 10, 10), 1, 1);
    drawPixel(23, 12, (nowSecond % 2)? Color(5, 5, 5) : Color(0, 0, 0));
    drawPixel(23, 14, (nowSecond % 2)? Color(5, 5, 5) : Color(0, 0, 0));
    swapBuffers(false);
  }
  lastSecond = nowSecond;
  DEBUG("Leaving nite stuff\n");
}
