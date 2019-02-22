/************************************
This is a our graphics core library, for all our displays. 
We'll be adapting all the
existing libaries to use this core to make updating, support 
and upgrading easier!

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above must be included in any redistribution
****************************************/


#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include "gui/gui.h"

/* helper functions */

#define swap(x, y) {int __tmp = x; x = y; y = __tmp;}

void LCD_DrawFastVLine(int16_t x, int16_t y, 
                 int16_t h, uint16_t color) {
    LCD_DrawStart(x, y, x, y + h -1, DRAW_NWSE); // bug fix: should be y+ h-1
    while(h--)
        LCD_DrawPixel(color);
    LCD_DrawStop();
}

void LCD_DrawFastHLine(u16 x, u16 y, u16 w, u16 color) {
    LCD_DrawStart(x, y, x + w -1, y, DRAW_NWSE);
    while(w--)
        LCD_DrawPixel(color);
    LCD_DrawStop();
}

void LCD_DrawDashedHLine(int16_t x, int16_t y, 
             int16_t w, int16_t space, uint16_t color)
{
    LCD_DrawStart(x, y, x + w -1, y, DRAW_NWSE);
    int16_t x1;
    for (x1 = 0; x1 < w; x1++)
        if ((x1 / space) & 0x01)
            LCD_DrawPixelXY(x1 + x, y, color);
    LCD_DrawStop();
}

void LCD_DrawDashedVLine(int16_t x, int16_t y, 
             int16_t h, int16_t space, uint16_t color) {
    LCD_DrawStart(x, y, x, y + h -1, DRAW_NWSE);
    int16_t y1;
    for (y1 = 0; y1 < h; y1++)
        if ((y1 / space) & 0x01)
            LCD_DrawPixelXY(x, y1 + y, color);
    LCD_DrawStop();
}

struct circle {
    int f;
    int ddF_x;
    int ddF_y;
    int x;
    int y;
};

void _calcCircleHelper(struct circle *c)
{
    if (c->f >= 0) {
        c->y     -= 1;
        c->ddF_y += 2;
        c->f     += c->ddF_y;
    }
    c->x     += 1;
    c->ddF_x += 2;
    c->f     += c->ddF_x;
}
// used to do circles and roundrects!
void fillCircleHelper(int x0, int y0, int r,
                unsigned cornername, int delta, unsigned color) {
  struct circle c = {1 - r, 1, -2 * r, 0, r};

  while (c.x<c.y) {
    _calcCircleHelper(&c);
    if (cornername & 0x1) {
      LCD_DrawFastVLine(x0+c.x, y0-c.y, 2*c.y+1+delta, color);
      LCD_DrawFastVLine(x0+c.y, y0-c.x, 2*c.x+1+delta, color);
    }
    if (cornername & 0x2) {
      LCD_DrawFastVLine(x0-c.x, y0-c.y, 2*c.y+1+delta, color);
      LCD_DrawFastVLine(x0-c.y, y0-c.x, 2*c.x+1+delta, color);
    }
  }
}

void drawCircleHelper( int x0, int y0,
               int r, unsigned cornername, unsigned color) {
  struct circle c = {1 - r, 1, -2 * r, 0, r};

  while (c.x<c.y) {
    _calcCircleHelper(&c);
    if (cornername & 0x4) {
      LCD_DrawPixelXY(x0 + c.x, y0 + c.y, color);
      LCD_DrawPixelXY(x0 + c.y, y0 + c.x, color);
    } 
    if (cornername & 0x2) {
      LCD_DrawPixelXY(x0 + c.x, y0 - c.y, color);
      LCD_DrawPixelXY(x0 + c.y, y0 - c.x, color);
    }
    if (cornername & 0x8) {
      LCD_DrawPixelXY(x0 - c.y, y0 + c.x, color);
      LCD_DrawPixelXY(x0 - c.x, y0 + c.y, color);
    }
    if (cornername & 0x1) {
      LCD_DrawPixelXY(x0 - c.y, y0 - c.x, color);
      LCD_DrawPixelXY(x0 - c.x, y0 - c.y, color);
    }
  }
}

/* end of helper functions */

// draw a circle outline
void LCD_DrawCircle(u16 x0, u16 y0, u16 r, u16 color)
{
  struct circle c = {1 - r, 1, -2 * r, 0, r};

  LCD_DrawPixelXY(x0, y0+r, color);
  LCD_DrawPixelXY(x0, y0-r, color);
  LCD_DrawPixelXY(x0+r, y0, color);
  LCD_DrawPixelXY(x0-r, y0, color);

  while (c.x<c.y) {
    _calcCircleHelper(&c);
    LCD_DrawPixelXY(x0 + c.x, y0 + c.y, color);
    LCD_DrawPixelXY(x0 - c.x, y0 + c.y, color);
    LCD_DrawPixelXY(x0 + c.x, y0 - c.y, color);
    LCD_DrawPixelXY(x0 - c.x, y0 - c.y, color);
    LCD_DrawPixelXY(x0 + c.y, y0 + c.x, color);
    LCD_DrawPixelXY(x0 - c.y, y0 + c.x, color);
    LCD_DrawPixelXY(x0 + c.y, y0 - c.x, color);
    LCD_DrawPixelXY(x0 - c.y, y0 - c.x, color);
    
  }
}

void LCD_FillCircle(u16 x0, u16 y0, u16 r, u16 color)
{
    LCD_DrawFastVLine(x0, y0-r, 2*r+1, color);
    fillCircleHelper(x0, y0, r, 3, 0, color);
}


// bresenham's algorithm - thx wikpedia
void LCD_DrawLine(u16 x0, u16 y0, u16 x1, u16 y1, u16 color)
{
  int steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  int dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int err = dx / 2;
  int ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0<=x1; x0++) {
    if (steep) {
      LCD_DrawPixelXY(y0, x0, color);
    } else {
      LCD_DrawPixelXY(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

// draw a rectangle
void LCD_DrawRect(u16 x, u16 y, u16 w, u16 h, u16 color)
{
  LCD_DrawFastHLine(x, y, w, color);
  LCD_DrawFastHLine(x, y+h-1, w, color);
  LCD_DrawFastVLine(x, y, h, color);
  LCD_DrawFastVLine(x+w-1, y, h, color);
}

void LCD_FillRect(u16 x, u16 y, u16 w, u16 h, u16 color)
{
    u32 bytes = (u32)w * h;
    LCD_DrawStart(x, y, x + w - 1, y + h -1, DRAW_NWSE); // Bug fix: should be y+h-1 instead of y+h
    while(bytes--)
        LCD_DrawPixel(color);
    LCD_DrawStop();
}

// draw a rounded rectangle!
void LCD_DrawRoundRect(u16 x, u16 y, u16 w, u16 h, u16 r, u16 color)
{
  // smarter version
  LCD_DrawFastHLine(x+r  , y    , w-2*r, color); // Top
  LCD_DrawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
  LCD_DrawFastVLine(  x    , y+r  , h-2*r, color); // Left
  LCD_DrawFastVLine(  x+w-1, y+r  , h-2*r, color); // Right
  // draw four corners
  drawCircleHelper(x+r    , y+r    , r, 1, color);
  drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
  drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}

// fill a rounded rectangle!
void LCD_FillRoundRect(u16 x, u16 y, u16 w, u16 h, u16 r, u16 color)
{
  // smarter version
  LCD_FillRect(x+r, y, w-2*r, h, color);

  // draw four corners
  fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}

// draw a triangle!
void LCD_DrawTriangle(u16 x0, u16 y0, u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
  LCD_DrawLine(x0, y0, x1, y1, color);
  LCD_DrawLine(x1, y1, x2, y2, color);
  LCD_DrawLine(x2, y2, x0, y0, color);
}

// fill a triangle!
void LCD_FillTriangle(u16 x0, u16 y0, u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
  int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }
  if (y1 > y2) {
    swap(y2, y1); swap(x2, x1);
  }
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }

  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a)      a = x1;
    else if(x1 > b) b = x1;
    if(x2 < a)      a = x2;
    else if(x2 > b) b = x2;
    LCD_DrawFastHLine(a, y0, b-a+1, color);
    return;
  }

  int
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1,
    sa   = 0,
    sb   = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if(y1 == y2) last = y1;   // Include y1 scanline
  else         last = y1-1; // Skip it

  for(y=y0; y<=last; y++) {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) swap(a,b);
    LCD_DrawFastHLine(a, y, b-a+1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y<=y2; y++) {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) swap(a,b);
    LCD_DrawFastHLine(a, y, b-a+1, color);
  }
}

void LCD_DrawRLE(const u8 *data, int len, u32 color)
{
    while(len) {
        u32 c = (*data & 0x80) ? color : 0;
        for(int i = 0; i < (*data & 0x7f); i++) {
            LCD_DrawPixel(c);
        }
        data++;
        len--;
    }
}

#ifndef LCD_DrawUSBLogo
extern u8 usb_logo[];
void LCD_DrawUSBLogo(int lcd_width, int lcd_height)
{
    int width = (usb_logo[0] << 8) |  usb_logo[1];
    int height= (usb_logo[2] << 8) |  usb_logo[3];
    int size  = (usb_logo[4] << 8) |  usb_logo[5];
    int x = (lcd_width - width) / 2;
    int y = (lcd_height - height) / 2;
    LCD_DrawStart(x, y, x + width-1, y + height, DRAW_NWSE);
    LCD_DrawRLE(usb_logo+6, size, 0xffff);
    LCD_DrawStop();
}
#endif

#ifdef USE_PBM_IMAGE
    #include "lcd_pbm_image.h"
#else
#ifdef USE_GIF_IMAGE
    #include "lcd_gif_image.h"
#else
    #include "lcd_bmp_image.h"
#endif
#endif

void LCD_DrawImageFromFile(u16 x, u16 y, const char *file)
{
    LCD_DrawWindowedImageFromFile(x, y, file, -1, -1, 0, 0);
}
