//BITMAPS - really wasted space, must shrink size for each layer of image
//and place it correctly rather than making each layer same size.
/* blinky bitmap file for GLCD library */
/* Bitmap created from blinky.png      */
/* Date: 25 Sep 2013      */
/* Image Pixels = 256    */
/* Image Bytes  = 32     */

#include <inttypes.h>
//#include <avr/pgmspace.h>

#ifndef blinky_H
#define blinky_H

static uint8_t blinky[]  = { 
0x3, 0xc0, 0xf, 0xf0, 0x1f, 0xf8, 0x3f, 0xfc, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7b, 0xde, 
0x31, 0x8c, 
 };

static uint8_t blinky2[]  ={ 
0x3, 0xc0, 0xf, 0xf0, 0x1f, 0xf8, 0x3f, 0xfc, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xde, 0x7b, 
0x8c, 0x31, 
 };

static uint8_t scared[]  = { 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0x60, 0x6, 
0x60, 0x0, 0x0, 0x0, 0x0, 0x19, 0x98, 0x26, 0x64, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 
 };

static uint8_t eyes1[]  = { 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0x18, 0xf, 0x3c, 0xf, 0x3c, 0xf, 
0x3c, 0x6, 0x18, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0
 };
 
static uint8_t eyes2[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3, 0xc, 0x3, 
0xc, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0
 };

static uint8_t pac[] = {
0x0, 0x0, 0x3, 0xe0, 0xf, 0xf8, 0x1f, 0xfc, 0x3f, 0xfe, 0x3f, 0xfe, 0x7f, 0xfc, 0x7f, 
0xf0, 0x7f, 0x80, 0x7f, 0xf0, 0x7f, 0xfc, 0x3f, 0xfe, 0x3f, 0xfe, 0x1f, 0xfc, 0xf, 0xf8, 
0x3, 0xe0
};

static uint8_t pac2[]  = { 
0x0, 0x0, 0x3, 0xe0, 0xf, 0xf8, 0x1f, 0xfc, 0x3f, 0xfe, 0x3f, 0xfe, 0x7f, 0xff, 0x7f, 
0xf8, 0x7f, 0x80, 0x7f, 0xf8, 0x7f, 0xff, 0x3f, 0xfe, 0x3f, 0xfe, 0x1f, 0xfc, 0xf, 0xf8, 
0x3, 0xe0, 
 };
 
static uint8_t pac3[]  = { 
0x0, 0x0, 0x3, 0xe0, 0xf, 0xf8, 0x1f, 0xfc, 0x3f, 0xfe, 0x3f, 0xfe, 0x7f, 0xff, 0x7f, 
0xff, 0x7f, 0xff, 0x7f, 0xff, 0x7f, 0xff, 0x3f, 0xfe, 0x3f, 0xfe, 0x1f, 0xfc, 0xf, 0xf8, 
0x3, 0xe0, 
 };

static uint8_t pac_left[]  = { 
0x0, 0x0, 0x7, 0xc0, 0x1f, 0xf0, 0x3f, 0xf8, 0x7f, 0xfc, 0x7f, 0xfc, 0x3f, 0xfe, 0xf, 
0xfe, 0x1, 0xfe, 0xf, 0xfe, 0x3f, 0xfe, 0x7f, 0xfc, 0x7f, 0xfc, 0x3f, 0xf8, 0x1f, 0xf0, 
0x7, 0xc0, 
 };

static uint8_t pac_left2[]  = { 
0x0, 0x0, 0x7, 0xc0, 0x1f, 0xf0, 0x3f, 0xf8, 0x7f, 0xfc, 0x7f, 0xfc, 0xff, 0xfe, 0x1f, 
0xfe, 0x1, 0xfe, 0x1f, 0xfe, 0xff, 0xfe, 0x7f, 0xfc, 0x7f, 0xfc, 0x3f, 0xf8, 0x1f, 0xf0, 
0x7, 0xc0, 
 };

static uint8_t pac_left3[]  = { 
0x0, 0x0, 0x7, 0xc0, 0x1f, 0xf0, 0x3f, 0xf8, 0x7f, 0xfc, 0x7f, 0xfc, 0xff, 0xfe, 0xff, 
0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0x7f, 0xfc, 0x7f, 0xfc, 0x3f, 0xf8, 0x1f, 0xf0, 
0x7, 0xc0, 
 };
 

static uint8_t cake_main[] = { 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x61, 0x43, 0x0, 0x0, 0xc6, 
0x31, 0x80, 0x8, 0x47, 0xf1, 0x8, 0xf, 0x80, 0x0, 0xf8, 0xf, 0xff, 0xff, 0xf8, 0xf, 
0xff, 0xff, 0xf8, 0xf, 0xff, 0xff, 0xf8, 0xf, 0xff, 0xff, 0xf8, 0x7, 0xff, 0xff, 0xf0, 
0x0, 0x7f, 0xff, 0x0, 
 };
 
static uint8_t cake_top[] = { 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x71, 0x47, 0x0, 0x0, 0x80, 0x0, 0x80, 0x8, 0x0, 
0x0, 0x8, 0x7, 0x80, 0x0, 0xf0, 0x0, 0x7f, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
 };
 
static uint8_t cake_cream[] = { 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe, 0x38, 0x0, 0x7, 0x0, 
0x0, 0x70, 0x0, 0x38, 0xe, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
 };

static uint8_t cake_cherries[] = { 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x4, 0x10, 0x0, 0x2, 0xe, 0x38, 0x20, 0x7, 0x10, 0x4, 0x70, 0x0, 0x38, 
0xe, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
 };

static uint8_t cake_candle[]  = { 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x80, 
0x0, 0x0, 0x0, 0x80, 0x0, 0x0, 0x0, 0x80, 0x0, 0x0, 0x0, 0x80, 0x0, 0x0, 0x1, 
0xc0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
 };

static uint8_t cake_flame[] = { 
0x0, 0x0, 0x80, 0x0, 0x0, 0x0, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
 };

static uint8_t cloud[]  = { 
0x0, 0x0, 0x0, 0xe0, 0x1, 0xf8, 0x3b, 0xfc, 0x7f, 0xfe, 0xff, 0xff, 0xff, 0xff, 0x7f, 
0xff, 0x3f, 0xfe, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 
 };

static uint8_t small_sun[] = { 
0x0, 0x0, 0xe, 0x0, 0x3f, 0x80, 0x3f, 0x80, 0x7f, 0xc0, 0x7f, 0xc0, 0x7f, 0xc0, 0x3f, 
0x80, 0x3f, 0x80, 0xe, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 
 };

static uint8_t big_sun[] = { 
0x0, 0x0, 0x81, 0x2, 0x41, 0x4, 0x20, 0x8, 0x3, 0x80, 0xf, 0xe0, 0xf, 0xe0, 0x1f, 
0xf0, 0xdf, 0xf6, 0x1f, 0xf0, 0xf, 0xe0, 0xf, 0xe0, 0x3, 0x80, 0x20, 0x8, 0x41, 0x4, 
0x81, 0x2, 
 };
 
static uint8_t lightning[]  = { 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x1, 0xc0, 0x0, 0xe0, 0x0, 0x70, 0x0, 0xe0, 0x1, 0xc0, 0x3, 0x0, 
0x2, 0x0, 
 };
 
static uint8_t cloud_outline[] = { 
0x0, 0x0, 0x0, 0xe0, 0x1, 0x18, 0x3a, 0x4, 0x4e, 0x2, 0x80, 0x1, 0x80, 0x1, 0x40, 
0x1, 0x3f, 0xfe, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 
 };

static uint8_t pumpkin_body[] = { 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x70, 0xe, 0x0, 0x0, 0xfe, 0x7f, 
0x0, 0x1, 0xff, 0xff, 0x80, 0x3, 0xff, 0xff, 0xc0, 0x3, 0xff, 0xff, 0xc0, 0x7, 0xff, 
0xff, 0xe0, 0x7, 0xff, 0xff, 0xe0, 0x7, 0xff, 0xff, 0xe0, 0x7, 0xff, 0xff, 0xe0, 0x3, 
0xff, 0xff, 0xc0, 0x3, 0xff, 0xff, 0xc0, 0x1, 0xff, 0xff, 0x80, 0x0, 0xff, 0xff, 0x0, 
0x0, 0x1f, 0xf8, 0x0, 
 };

static uint8_t pumpkin_ribs[] = { 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x10, 0x8, 
0x0, 0x0, 0x22, 0x44, 0x0, 0x0, 0x44, 0x22, 0x0, 0x0, 0x44, 0x22, 0x0, 0x0, 0x44, 
0x22, 0x0, 0x0, 0x44, 0x22, 0x0, 0x0, 0x44, 0x22, 0x0, 0x0, 0x44, 0x22, 0x0, 0x0, 
0x44, 0x22, 0x0, 0x0, 0x22, 0x44, 0x0, 0x0, 0x11, 0x88, 0x0, 0x0, 0x9, 0x90, 0x0, 
0x0, 0x7, 0xe0, 0x0, 
 };

static uint8_t pumpkin_face[] = { 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x8, 0x10, 0x0, 0x0, 0x1c, 0x38, 0x0, 0x0, 0x3e, 
0x7c, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x81, 0x81, 0x0, 0x0, 0xc1, 0x83, 0x0, 0x0, 
0xf0, 0xf, 0x0, 0x0, 0x7d, 0xbe, 0x0, 0x0, 0x1f, 0xf8, 0x0, 0x0, 0x7, 0xe0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
 };
 
static uint8_t pumpkin_top[]  = { 
0x0, 0x0, 0xc0, 0x0, 0x0, 0x1, 0x80, 0x0, 0x0, 0x3, 0xc0, 0x0, 0x0, 0x1, 0x80, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
 };
 
static uint8_t boo[]  = { 
0x0, 0x0, 0x1, 0xc3, 0x7f, 0x1, 0x83, 0xe3, 0x3f, 0x83, 0xc3, 0xf3, 0x39, 0xc7, 0xe3, 
0x33, 0x39, 0xc6, 0x67, 0x33, 0x3b, 0x8c, 0x37, 0x1b, 0x3f, 0xc, 0x36, 0x1b, 0x3f, 0x8c, 
0x36, 0x1b, 0x39, 0xcc, 0x36, 0x19, 0x38, 0xec, 0x36, 0x19, 0x38, 0xec, 0x36, 0x39, 0x38, 
0xee, 0x63, 0x39, 0x38, 0xe7, 0xe3, 0x30, 0x3f, 0xc3, 0xc3, 0xf0, 0xff, 0x81, 0x81, 0xf3, 
0x0, 0x0, 0x0, 0xe3, 
 };
 
static uint8_t tree[] = { 
0x1, 0x80, 0x3, 0xc0, 0x7, 0xe0, 0xf, 0xf0, 0x3, 0xc0, 0x7, 0xe0, 0xf, 0xf0, 0x1f, 
0xf8, 0x3f, 0xfc, 0x7, 0xe0, 0xf, 0xf0, 0x1f, 0xf8, 0x3f, 0xfc, 0x7f, 0xfe, 0x0, 0x0, 
0x0, 0x0, 
 };
 
static uint8_t bow[]  = { 
0x0, 0x0, 0x38, 0x1c, 0x44, 0x22, 0x23, 0xc4, 0x1f, 0xf8, 0x1, 0x80, 0x1, 0x80, 0x1, 
0x80, 0x1, 0x80, 0x3f, 0xfc, 0x3f, 0xfc, 0x1, 0x80, 0x1, 0x80, 0x1, 0x80, 0x1, 0x80, 
0x0, 0x0, 
 };
#endif
