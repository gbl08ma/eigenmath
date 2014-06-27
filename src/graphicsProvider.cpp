#include <fxcg/display.h>
#include <fxcg/file.h>
#include <fxcg/keyboard.h>
#include <fxcg/system.h>
#include <fxcg/misc.h>
#include <fxcg/app.h>
#include <fxcg/serial.h>
#include <fxcg/rtc.h>
#include <fxcg/heap.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "graphicsProvider.hpp"

const short empty[18] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
extern int enable_bracket_coloring;
extern int last_bracket_color;
extern int bracket_level;
// the following routine is very heavily hacked, er, modified to accomodate this add-in's needs
int PrintMiniFix( int x, int y, const char*Msg, const int flags, const short color, const short bcolor )
{
  int i = 0, dx;
  unsigned short width;
  void*p;

  while ( Msg[ i ] )
  {
    if( Msg[i] == 7) {
      // fraction start
      drawLine(x+5, y+24+7, x+11, y+24+7, color);
      drawLine(x+5, y+24+8, x+11, y+24+8, color);
      x+=12;
      i++;
      continue;
    } else if( Msg[i] == 8) {
      // fraction end
      drawLine(x, y+24+7, x+11-5, y+24+7, color);
      drawLine(x, y+24+8, x+11-5, y+24+8, color);
      x+=12;
      i++;
      continue;
    } else if( Msg[i] == 9) {
      // fraction middle
      drawLine(x, y+24+7, x+11, y+24+7, color);
      drawLine(x, y+24+8, x+11, y+24+8, color);
      x+=12;
      i++;
      continue;
    } else if( Msg[i] == 30) {
      // getline start indicator
      int tx = x+2, ty=y;
      PrintMini(&tx, &ty, (unsigned char*)"\xe6\x9e", 0, 0xFFFFFFFF, 0, 0, COLOR_BLUE, bcolor, 1, 0);
      x+=12;
      i++;
      continue;
    } else if( Msg[i] == 31) {
      // small dot for multiply
      int tx = x+2, ty=y;
      PrintMini(&tx, &ty, (unsigned char*)"\xe6\xaa", 0, 0xFFFFFFFF, 0, 0, color, bcolor, 1, 0);
      x+=12;
      i++;
      continue;
    } else p = GetMiniGlyphPtr( Msg[ i ], &width );
    dx = ( 12 - width ) / 2;
    if ( dx > 0 )
    {
      PrintMiniGlyph( x, y, (void*)empty, flags, dx, 0, 0, 0, 0, color, bcolor, 0 );
    }
   
    else dx = 0;
    int actualcolor = color;
    if(enable_bracket_coloring) {
      if(Msg[ i ] == '(') {
        bracket_level++;
        actualcolor = last_bracket_color;
        last_bracket_color = getNextColorInSequence(last_bracket_color);
      } else if(Msg[ i ] == ')' && bracket_level > 0) {
        last_bracket_color = getPreviousColorInSequence(last_bracket_color);
        actualcolor = last_bracket_color;
        bracket_level--;
      }
    }
    PrintMiniGlyph( x + dx, y, p, flags, width, 0, 0, 0, 0, actualcolor, bcolor, 0 );
    if ( width + dx < 12 )
    {
      PrintMiniGlyph( x + width + dx, y, (void*)empty, flags, 12 - width - dx, 0, 0, 0, 0, color, bcolor, 0 );
    }
    x += 12;
    i++;
  }
  return x;
}

//draws a point of color color at (x0, y0) 
void plot(int x0, int y0,unsigned short color) {
  unsigned short* VRAM = (unsigned short*)0xA8000000;
  VRAM += (y0*LCD_WIDTH_PX + x0);
  *VRAM=color;
}

void drawRectangle(int x, int y, int width, int height, unsigned short color){
  unsigned short*VRAM = (unsigned short*)0xA8000000;
  VRAM+=(y*384)+x;
  while(height--){
    int i=width;
    while(i--){
      *VRAM++ = color;
    }
    VRAM+=384-width;
  }
}
//Uses the Bresenham line algorithm 
void drawLine(int x1, int y1, int x2, int y2, int color) { 
    signed char ix; 
    signed char iy; 
  
    // if x1 == x2 or y1 == y2, then it does not matter what we set here 
    int delta_x = (x2 > x1?(ix = 1, x2 - x1):(ix = -1, x1 - x2)) << 1; 
    int delta_y = (y2 > y1?(iy = 1, y2 - y1):(iy = -1, y1 - y2)) << 1; 
  
   plot(x1, y1, color);  
    if (delta_x >= delta_y) { 
        int error = delta_y - (delta_x >> 1);        // error may go below zero 
        while (x1 != x2) { 
            if (error >= 0) { 
                if (error || (ix > 0)) { 
                    y1 += iy; 
                    error -= delta_x; 
                }                           // else do nothing 
         }                              // else do nothing 
            x1 += ix; 
            error += delta_y; 
            plot(x1, y1, color); 
        } 
    } else { 
        int error = delta_x - (delta_y >> 1);      // error may go below zero 
        while (y1 != y2) { 
            if (error >= 0) { 
                if (error || (iy > 0)) { 
                    x1 += ix; 
                    error -= delta_y; 
                }                           // else do nothing 
            }                              // else do nothing 
            y1 += iy; 
            error += delta_x;  
            plot(x1, y1, color); 
        } 
    } 
}
//ReplaceColor By Kerm:
/*void VRAMReplaceColorInRect(int x, int y, int width, int height, color_t color_old, color_t color_new) { 
   //color_t* VRAM = GetVRAMAddress();
   color_t* VRAM = (color_t*)0xA8000000; 
   VRAM += (y*LCD_WIDTH_PX)+x; 
   for(int j=0; j<height; VRAM += (LCD_WIDTH_PX-width), j++) { 
      for(int i=0; i<width; VRAM++, i++) { 
         if (*VRAM == color_old) *VRAM = color_new; 
      } 
   } 
} */
/*void CopySprite(const void* datar, int x, int y, int width, int height) { 
   color_t*data = (color_t*) datar; 
   color_t* VRAM = (color_t*)0xA8000000; 
   VRAM += LCD_WIDTH_PX*y + x; 
   for(int j=y; j<y+height; j++) { 
      for(int i=x; i<x+width; i++) { 
         *(VRAM++) = *(data++); 
     } 
     VRAM += LCD_WIDTH_PX-width; 
   } 
}*/
void CopySpriteMasked(unsigned short* data, int x, int y, int width, int height, unsigned short maskcolor) {
  unsigned short* VRAM = (unsigned short*)0xA8000000; 
  VRAM += (LCD_WIDTH_PX*y + x); 
  while(height--) {
    int i=width;
    while(i--){
      if(*data!=maskcolor) {
        *(VRAM++) = *(data++);
      } else {
        ++VRAM;
        ++data;
      }
    }
    VRAM += (LCD_WIDTH_PX-width);
  }
}
/*void CopySpriteNbit(const unsigned char* data, int x, int y, int width, int height, const color_t* palette, unsigned int bitwidth) {
   color_t* VRAM = (color_t*)0xA8000000;
   VRAM += (LCD_WIDTH_PX*y + x);
   int offset = 0;
   unsigned char buf = 0;
   for(int j=y; j<y+height; j++) {
      int availbits = 0;
      for(int i=x; i<x+width;  i++) {
         if (!availbits) {
            buf = data[offset++];
            availbits = 8;
         }
         color_t thisthis = ((color_t)buf>>(8-bitwidth));
         *VRAM = palette[(color_t)thisthis];
         VRAM++;
         buf<<=bitwidth;
         availbits-=bitwidth;
      }
      VRAM += (LCD_WIDTH_PX-width);
   }
}*/
//the following does not update the screen automatically; it will draw the tny.im logo starting at screen coordinates x,y
//the tny.im logo is great enough not to require any sprites! yay!
//w:138
//h:42
static const unsigned char logoB[]={
//draw t
0, 6, 6, 24,
6, 12, 6, 6,
6, 30, 6, 6,
//draw n
18, 12, 6, 24,
24, 12, 12, 6,
36, 18, 6, 18,
//draw y
48, 12, 6, 18,
60, 12, 6, 18,
54, 30, 6, 6,
48, 36, 6, 6,
//draw dot
72, 30, 6, 6 };
static const unsigned char logoO[]={
//draw i (orange)
84, 0, 6, 6,
84, 12, 6, 24,
//draw m (orange)
96, 12, 6, 24,
102, 12, 12, 6,
114, 18, 6, 18,
120, 12, 12, 6,
132, 18, 6, 18 };
void drawtnyimLogo(int x, int y) {
  int i;
  for(i=0;i<11*4;i+=4)
    drawRectangle(x+logoB[i], y+logoB[i+1], logoB[i+2], logoB[i+3], COLOR_BLACK);
  for(i=0;i<7*4;i+=4)
    drawRectangle(x+logoO[i], y+logoO[i+1], logoO[i+2], logoO[i+3], TNYIM_ORANGE);
}

/*int textColorToFullColor(int textcolor) {
  switch(textcolor) {
    case TEXT_COLOR_BLACK: return COLOR_BLACK;
    case TEXT_COLOR_BLUE: return COLOR_BLUE;
    case TEXT_COLOR_GREEN: return COLOR_GREEN;
    case TEXT_COLOR_CYAN: return COLOR_CYAN;
    case TEXT_COLOR_RED: return COLOR_RED;
    case TEXT_COLOR_PURPLE: return COLOR_PURPLE;
    case TEXT_COLOR_YELLOW: return COLOR_YELLOW;
    case TEXT_COLOR_WHITE: return COLOR_LIGHTGRAY;
    default: return COLOR_BLACK;
  }
}

void progressMessage(char* message, int cur, int total) {
  char buffer[30] = "";
  char buffer2[5] = "";
  strcpy(buffer, "  ");
  strcat(buffer, message);
  strcat(buffer, " (");
  itoa(cur, (unsigned char*)buffer2);
  strcat(buffer, buffer2);
  strcat(buffer, "/");
  itoa(total, (unsigned char*)buffer2);
  strcat(buffer, buffer2);
  strcat(buffer, ")");
  PrintXY(1,8,(char*)"                        ", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
  PrintXY(1,8,(char*)buffer, TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
  Bdisp_PutDisp_DD();
}*/

void printCentered(unsigned char* text, int y, int FGC, int BGC) {
  int len = strlen((char*)text);
  int x = LCD_WIDTH_PX/2-(len*18)/2;
  int cur = 0;
  while(cur<len) {
    PrintCXY(x, y, &text[cur], 0x40, -1, FGC, BGC, 1, 0 );
    x=x+18;
    cur++;
  }
}

void clearLine(int x, int y, color_t color) {
  // clear text line. x and y are text cursor coordinates
  // this is meant to achieve the same effect as using PrintXY with a line full of spaces (except it doesn't waste strings).
  int width=LCD_WIDTH_PX;
  if(x>1) width = 24*(21-x);
  drawRectangle((x-1)*18, y*24, width, 24, color);
}

void mPrintXY(int x, int y, char*msg, int mode, int color) {
  char nmsg[50];
  nmsg[0] = 0x20;
  nmsg[1] = 0x20;
  nmsg[2] = '\0';
  strncat(nmsg, msg, 48);
  PrintXY(x, y, nmsg, mode, color );
}

void drawScreenTitle(char* title, char* subtitle) {
  if(title != NULL) mPrintXY(1, 1, title, TEXT_MODE_TRANSPARENT_BACKGROUND, TEXT_COLOR_BLUE);
  if(subtitle != NULL) mPrintXY(1, 2, subtitle, TEXT_MODE_TRANSPARENT_BACKGROUND, TEXT_COLOR_BLACK);
}

void drawFkeyLabels(int f1, int f2, int f3, int f4, int f5, int f6) {
  // set arguments to negative numbers if that fkey is not to be affected.
  int iresult;
  if(f1>=0) {
    GetFKeyPtr(f1, &iresult);
    FKey_Display(0, (int*)iresult);
  }
  if(f2>=0) {
    GetFKeyPtr(f2, &iresult);
    FKey_Display(1, (int*)iresult);
  }
  if(f3>=0) {
    GetFKeyPtr(f3, &iresult);
    FKey_Display(2, (int*)iresult);
  }
  if(f4>=0) {
    GetFKeyPtr(f4, &iresult);
    FKey_Display(3, (int*)iresult);
  }
  if(f5>=0) {
    GetFKeyPtr(f5, &iresult);
    FKey_Display(4, (int*)iresult);
  }
  if(f6>=0) {
    GetFKeyPtr(f6, &iresult);
    FKey_Display(5, (int*)iresult);
  }
}

int getNextColorInSequence(int curcolor) {
  switch(curcolor) {
    case COLOR_BLUE: return COLOR_RED;
    case COLOR_RED: return COLOR_LIME;
    case COLOR_LIME: return COLOR_MAGENTA;
    case COLOR_MAGENTA: return COLOR_BLACK;
    case COLOR_BLACK:
    default:
      return COLOR_BLUE;
  }
}

int getPreviousColorInSequence(int curcolor) {
  switch(curcolor) {
    case COLOR_BLUE: return COLOR_BLACK;
    case COLOR_RED: return COLOR_BLUE;
    case COLOR_LIME: return COLOR_RED;
    case COLOR_MAGENTA: return COLOR_LIME;
    case COLOR_BLACK:
    default:
      return COLOR_MAGENTA;
  }
}