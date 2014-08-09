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
extern "C" {
#include "dConsole.h"
}
#include "catalogGUI.hpp"
#include "aboutGUI.hpp"
#include "menuGUI.hpp"
#include "inputGUI.hpp"
#include "defs.h"

#include "graphicsProvider.hpp"
#include "fileProvider.hpp"
#include "stringsProvider.hpp"
#include "constantsProvider.hpp"

typedef unsigned int    uint;
typedef unsigned char   uchar;

typedef char line_row[LINE_COL_MAX+1];
line_row *line_buf;
int line_index = 0;
int line_x     = 0;
int line_start = 0;
int line_count = 0;

int myconsolex = 0;
int myconsoley = 0;
int myconsolescroll = 0;

char* outputRedirectBuffer = NULL; // if not null, console output will be redirected to the buffer this char* points to.
int remainingBytesInRedirect = 0; // bytes remaining in the redirect buffer, respect to avoid overflows

void initializeConsoleMemory(line_row* area) {
  line_buf = area;
}

void locate(int x, int y) {
  myconsolex = x-1;
  myconsoley = y-1;
}
int enable_bracket_coloring = 0;
int last_bracket_color = COLOR_BLUE;
int bracket_level = 0;
void print(unsigned char* msg) {
  int linestart = line_count-10+myconsolescroll;
  if(linestart < 0) linestart = 0;
  if(myconsoley - linestart >= 0 && myconsoley - linestart < 10) PrintMiniFix( myconsolex*12, (myconsoley - linestart)*17, msg, 0, COLOR_BLACK, COLOR_WHITE );
  myconsolex = myconsolex + strlen((char*)msg);
}

char dGetKeyChar (uint key)
{
  if (key>=KEY_CHAR_A && key<=KEY_CHAR_Z)
          return key;
  if (key>=KEY_CHAR_LOWER_A && key<=KEY_CHAR_LOWER_Z)
          return key;
  else if (key>=KEY_CHAR_0 && key<= KEY_CHAR_9)
          return key;
  else if (key>=' ' && key<='~')
          return key;
  switch(key)
  {
    case KEY_CHAR_PLUS:
    case KEY_CHAR_MINUS:
    case KEY_CHAR_MULT:
    case KEY_CHAR_DIV:
    case KEY_CHAR_POW:
      return key;
  }

  return 0;
}

void dConsoleCls ()
{
  line_index      = 0;
  line_x          = 0;
  line_start      = 0;
  line_count      = 0;
  myconsolescroll = 0;
  dConsoleRedraw();
}

void printCursor() {
  int x = myconsolex*12;
  int linestart = line_count-10+myconsolescroll;
  if(linestart < 0) linestart = 0;
  if(!(myconsoley - linestart >= 0 && myconsoley - linestart < 10)) return;
  int y = (myconsoley-linestart)*17+24;
  // vertical cursor...
  drawLine(x, y+14, x, y, COLOR_BLACK);
  drawLine(x+1, y+14, x+1, y, COLOR_BLACK); 
}

void do_up_arrow(void);
void do_down_arrow(void);
extern int custom_key_to_handle;
extern int custom_key_to_handle_modifier;
int get_custom_key_handler_state();
int is_running_in_strip();
int get_set_session_setting(int value);

// inserts into subject at position pos. assumes subject has enough space!
//append(s, (char*)"tan(", pos);
void append(char* subject, const char* insert, int pos, int inslen) {
  char buf[INPUTBUFLEN+5];
  strcpy(buf, subject+pos); // backup everything in the subject, after pos
  strcpy(subject+pos, insert); // copy string to insert into subject at desired position
  strcpy(subject+pos+inslen, buf); // restore backup at the end of the inserted string
}

void addStringToInput(char* dest, char* src, int* pos, int max, int* refresh) {
  int srclen = strlen(src);
  if ((int)strlen(dest)+srclen-1>=max) return;
  append(dest, src, *pos, srclen);
  *pos+=srclen; *refresh = 1;
}

int eigenmathRanAtLeastOnce = 0;
int dGetLine (char * s,int max, int isRecording) {
  int pos = strlen(s);
  int start = 0;
  int refresh = 1;
  int x,y,l,width;
  int key;
  char c;

  l = strlen(line_buf[line_index]);

  if (l>=LINE_COL_MAX) {
    dConsolePut("\n");
    l = 0;
  } else dConsoleRedraw();

  x = l + 1;
  y = line_count;
  width = LINE_COL_MAX - l;
  int firstLoopRun = 1;
  int isscrolling = 0;
  while (1) {
    if(pos-1<start) {
      do start--; while (pos-1<start);
      if (start<0) start = 0;
    } else if(pos>start+width-1) {
      do start++; while (pos>start+width-1);
    }
    if(isscrolling) {
      DefineStatusMessage((char*)"Scrolling enabled (up/down)", 1, 0, 0);
      DisplayStatusArea();
    }
    if (refresh) {
      int i;
      for (i=x;i<=LINE_COL_MAX;++i) {
        locate(i,y); print((uchar*)" ");
      }
      last_bracket_color = COLOR_BLUE;
      bracket_level = 0;
      if (start==0) {
        enable_bracket_coloring = 1;
        locate(x,y);          print((uchar*)s);
        enable_bracket_coloring = 0;
        locate(x+pos,y);      printCursor();
      } else {
        for(int i = 0; i < start; i++) {
          if(s[i] == '(') {
            bracket_level++;
            last_bracket_color = getNextColorInSequence(last_bracket_color);
          } else if(s[i] == ')' && bracket_level > 0) {
            last_bracket_color = getPreviousColorInSequence(last_bracket_color);
            bracket_level--;
          }
        }
        enable_bracket_coloring = 1;
        locate(x,y);          print((uchar*)s+start);
        enable_bracket_coloring = 0;
        locate(pos-start+2,y);  printCursor(); //cursor
      }
      if(!isscrolling) {
        int linestart = line_count-10+myconsolescroll;
        if(linestart < 0) linestart = 0;
        int py = (y-linestart-1)*17;
        if(start) {
          int px = 12;
          PrintMini(&px, &py, (unsigned char*)"\xe6\x9a", 0x02, 0xFFFFFFFF, 0, 0, COLOR_MAGENTA, COLOR_WHITE, 1, 0);
        }
        if((int)strlen(s+start)>width) {
          int px = 31*12;
          PrintMini(&px, &py, (unsigned char*)"\xe6\x9b", 0x02, 0xFFFFFFFF, 0, 0, COLOR_MAGENTA, COLOR_WHITE, 1, 0);
        }
      }
      refresh = 0;
    }
    int keyflag = GetSetupSetting( (unsigned int)0x14);
    DirectDrawRectangle(LCD_WIDTH_PX+6, 24, LCD_WIDTH_PX+6+5, 210, COLOR_WHITE); // clear scroll indicator
    if(isscrolling) {
      int starty = (line_count == 0 ? 0 : ((line_count+myconsolescroll-(line_count < 10 ? line_count : 10))*162)/(line_count < 10 ? line_count : line_count-10));
      DirectDrawRectangle(LCD_WIDTH_PX+6, 24+starty, LCD_WIDTH_PX+6+5, 24+starty+8, COLOR_BLACK);
    }
    GetKey(&key);
    if(isscrolling && !(key==KEY_CTRL_PAGEUP || key==KEY_CTRL_PAGEDOWN || key==KEY_CTRL_UP || key==KEY_CTRL_DOWN)) {
      isscrolling = 0;
      myconsolescroll = 0;
      DefineStatusMessage((char*)"", 1, 0, 0);
      dConsoleRedraw();
      refresh = 1;
    }
    if(key == KEY_CTRL_F5) {
      if (keyflag == 0x04 || keyflag == 0x08 || keyflag == 0x84 || keyflag == 0x88) {
        // ^only applies if some sort of alpha (not locked) is already on
        if (keyflag == 0x08 || keyflag == 0x88) { //if lowercase
          SetSetupSetting( (unsigned int)0x14, keyflag-0x04);
          DisplayStatusArea();
          continue; //do not process the key, because otherwise we will leave alpha status
        } else {
          SetSetupSetting( (unsigned int)0x14, keyflag+0x04);
          DisplayStatusArea();
          continue; //do not process the key, because otherwise we will leave alpha status
        }
      }
    } else if (key==KEY_CHAR_PLUS) {
      if ((int)strlen(s)>=max) continue;
      if(strlen(s)==0 && firstLoopRun) addStringToInput(s, "last+", &pos, max, &refresh); //start of line, append "last" as we're going to do a calculation on the previous value
      else addStringToInput(s, "+", &pos, max, &refresh);
    } else if (key==KEY_CHAR_MINUS) {
      if ((int)strlen(s)>=max) continue;
      if(strlen(s)==0 && firstLoopRun) addStringToInput(s, "last-", &pos, max, &refresh); //start of line, append "last" as we're going to do a calculation on the previous value
      else addStringToInput(s, "-", &pos, max, &refresh);
    } else if (key==KEY_CHAR_MULT) {
      if ((int)strlen(s)>=max) continue;
      if(strlen(s)==0 && firstLoopRun) addStringToInput(s, "last*", &pos, max, &refresh); //start of line, append "last" as we're going to do a calculation on the previous value
      else addStringToInput(s, "*", &pos, max, &refresh);
    } else if (key==KEY_CHAR_DIV) {
      if ((int)strlen(s)>=max) continue;
      if(strlen(s)==0 && firstLoopRun) addStringToInput(s, "last/", &pos, max, &refresh); //start of line, append "last" as we're going to do a calculation on the previous value
      else addStringToInput(s, "/", &pos, max, &refresh);
    } else if (key==KEY_CHAR_POW) {
      if ((int)strlen(s)>=max) continue;
      if(strlen(s)==0 && firstLoopRun) addStringToInput(s, "last^", &pos, max, &refresh); //start of line, append "last" as we're going to do a calculation on the previous value
      else addStringToInput(s, "^", &pos, max, &refresh);
    } else if (key==KEY_CHAR_SQUARE) {
      if ((int)strlen(s)+1>=max) continue;
      if(strlen(s)==0 && firstLoopRun) addStringToInput(s, "last^2", &pos, max, &refresh); //start of line, append "last" as we're going to do a calculation on the previous value
      else addStringToInput(s, "^2", &pos, max, &refresh);
    } else if (key==KEY_CHAR_ROOT) {
      addStringToInput(s, "sqrt(", &pos, max, &refresh);
    } else if (key==KEY_CHAR_CUBEROOT) {
      addStringToInput(s, "^(1/3)", &pos, max, &refresh);
    } else if (key==KEY_CHAR_POWROOT) {
      addStringToInput(s, "^(1/", &pos, max, &refresh);
    } else if (key==KEY_CHAR_THETA) {
      addStringToInput(s, "theta", &pos, max, &refresh);
    } else if (key==KEY_CTRL_XTT) {
      addStringToInput(s, "x", &pos, max, &refresh);
    } else if (key==KEY_CHAR_LN) { // the log() function in eigenmath is the natural log (Casio's ln)
      addStringToInput(s, "log(", &pos, max, &refresh);
    } else if (key==KEY_CHAR_EXPN) { // the exp() function in eigenmath is the natural expnonent (Casio's e^x)
      addStringToInput(s, "exp(", &pos, max, &refresh);
    } else if (key==KEY_CHAR_SIN) {
      addStringToInput(s, "sin(", &pos, max, &refresh);
    } else if (key==KEY_CHAR_ASIN) {
      addStringToInput(s, "arcsin(", &pos, max, &refresh);
    } else if (key==KEY_CHAR_COS) {
      addStringToInput(s, "cos(", &pos, max, &refresh);
    } else if (key==KEY_CHAR_ACOS) {
      addStringToInput(s, "arccos(", &pos, max, &refresh);
    } else if (key==KEY_CHAR_TAN) {
      addStringToInput(s, "tan(", &pos, max, &refresh);
    } else if (key==KEY_CHAR_ATAN) {
      addStringToInput(s, "arctan(", &pos, max, &refresh);
    } else if (key==KEY_CHAR_FRAC) {
      addStringToInput(s, "/", &pos, max, &refresh);
    } else if (key==KEY_CTRL_FD) {
      addStringToInput(s, "float", &pos, max, &refresh); // no ( at the end because this is often used to manipulate the last result
    } else if (key==KEY_CHAR_STORE) {
      if ((int)strlen(s)>=max) continue;
      if(strlen(s)==0 && firstLoopRun) addStringToInput(s, "last!", &pos, max, &refresh); //start of line, append "last" as we're going to do a calculation on the previous value
      else addStringToInput(s, "!", &pos, max, &refresh);
    } else if (key==KEY_CHAR_IMGNRY) {
      addStringToInput(s, "i", &pos, max, &refresh);
    } else if (key==KEY_CHAR_PI) {
      addStringToInput(s, "pi", &pos, max, &refresh);
    } else if (key==KEY_CHAR_EXP) {
      addStringToInput(s, "*10^(", &pos, max, &refresh);
    } else if (key==KEY_CHAR_PMINUS) {
      addStringToInput(s, "(-", &pos, max, &refresh);
    } else if (key==KEY_CHAR_ANS) {
      addStringToInput(s, "last", &pos, max, &refresh);
    } else if (key==KEY_CTRL_PASTE) {
      // paste contents to buffer using system syscalls, then append buffer to console input
      int nmax = max - strlen(s);
      if(nmax > 0) {
        char buffer[INPUTBUFLEN] = "";
        int start = 0;
        int cursor = 0;
        int ekey = KEY_CTRL_PASTE;
        EditMBStringCtrl2( (unsigned char*)buffer, nmax, &start, &cursor, &ekey, 1, 1*24-24, 1, 20 );
        Cursor_SetFlashOff();
        addStringToInput(s, buffer, &pos, max, &refresh);
        refresh = 1; // to redraw input even if pasted content is too long to be inserted
        dConsoleRedraw();
      }
    } else if (key==KEY_CTRL_CLIP) {
      // allow for copying last result or current write buffer
      MsgBoxPush(4);
      MenuItem smallmenuitems[5];
      smallmenuitems[0].text = (char*)"Last result";
      smallmenuitems[1].text = (char*)"Current command";
      
      Menu smallmenu;
      smallmenu.items=smallmenuitems;
      smallmenu.numitems=2;
      smallmenu.width=17;
      smallmenu.height=4;
      smallmenu.startX=3;
      smallmenu.startY=2;
      smallmenu.scrollbar=0;
      smallmenu.title = (char*)"Clip on:";
      int sres = doMenu(&smallmenu);
      MsgBoxPop();
      
      if(sres == MENU_RETURN_SELECTION) {
        if(smallmenu.selection == 1) {
          if(eigenmathRanAtLeastOnce) {
            int was_tty = 0;
            if (equaln(get_binding(symbol(TTY)), 1)) was_tty = 1;
            else run("tty=1");
            char buffer[INPUTBUFLEN] = "";
            outputRedirectBuffer = buffer;
            remainingBytesInRedirect = INPUTBUFLEN-1;
            run("eval(last)");
            outputRedirectBuffer = NULL;
            if(!was_tty) run("tty=0");
            int len = strlen(buffer);
            if(len) {
              buffer[len-1] = '\0'; // remove newline at end
              CLIP_Store((unsigned char*)buffer, len);
              DefineStatusMessage((char*)"Last result copied to clipboard", 1, 0, 0);
            }
          } else AUX_DisplayErrorMessage( 0x15 );
        } else if(smallmenu.selection == 2) {
          Bdisp_AllClr_VRAM();
          DisplayStatusArea();
          drawScreenTitle((char*)"Edit current command", (char*)"Shift+8 to clip");
          textInput input;
          input.charlimit=max;
          input.buffer = (char*)s;
          input.cursor = pos;
          doTextInput(&input);
          pos=input.cursor;
          start = 0; // force recalculation
        }
      }
      refresh = 1;
      dConsoleRedraw();
    } else if(key == KEY_CTRL_SETUP) {
      Menu smallmenu;
      MenuItem smallmenuitems[5];
      
      smallmenu.items=smallmenuitems;
      smallmenu.width=17;
      smallmenu.height=5;
      smallmenu.startX=3;
      smallmenu.startY=2;
      smallmenu.scrollbar=0;
      smallmenu.numitems=5;
      smallmenuitems[0].text = (char*)"Function Catalog";
      smallmenuitems[1].text = (char*)"Load Script";
      smallmenuitems[2].text = (char*)(isRecording ? "Stop Recording" : "Record Script");
      smallmenuitems[4].text = (char*)"About Eigenmath";
      while(1) {
        if(is_running_in_strip()) smallmenuitems[3].text = (char*)"Set Strip Script";
        else {
          smallmenuitems[3].type = MENUITEM_CHECKBOX;
          smallmenuitems[3].value = get_set_session_setting(-1);
          smallmenuitems[3].text = (char*)"Save Session";
        }
        MsgBoxPush(5);
        int sres = doMenu(&smallmenu);
        MsgBoxPop();
        
        if(sres == MENU_RETURN_SELECTION) {
          if(smallmenu.selection == 1) {
            // open functions catalog
            char text[20];
            if(showCatalog(text)) {
              addStringToInput(s, text, &pos, max, &refresh);
            } else refresh = 1;
            dConsoleRedraw();
            break;
          } else if(smallmenu.selection == 2) {
            return 2;
          } else if(smallmenu.selection == 3) {
            strcpy(s, "record");
            return 1;
          } else if(smallmenu.selection == 4) {
            if(is_running_in_strip()) return 4;
            else {
              get_set_session_setting(!get_set_session_setting(-1));
              continue;
            }
          } else if(smallmenu.selection == 5) {
            showAbout();
          }
        }
        refresh = 1;
        dConsoleRedraw();
        break;
      }
    } else if (key==KEY_CTRL_CATALOG) {
      // open functions catalog
      char text[20];
      if(showCatalog(text)) {
        addStringToInput(s, text, &pos, max, &refresh);
      } else refresh = 1;
      dConsoleRedraw();
    } else if (key==KEY_CTRL_UP) {
      if(isscrolling) {
        myconsolescroll--;
        if(line_count-10+myconsolescroll < 0) myconsolescroll++;
        dConsoleRedraw();
      } else {
        // go up in command history
        do_up_arrow();
        pos=strlen(s);
        start = 0; // force recalculation
      }
      refresh = 1;
    } else if (key==KEY_CTRL_DOWN) {
      // go down in command history
      if(isscrolling) {
        myconsolescroll++;
        if(myconsolescroll>0) myconsolescroll = 0;
        dConsoleRedraw();
      } else {
        do_down_arrow();
        pos=strlen(s);
        start = 0; // force recalculation
      }
      refresh = 1;
    } else if (key==KEY_CTRL_PAGEUP || key==KEY_CTRL_PAGEDOWN) {
      isscrolling = 1;
    } else if (key==KEY_CTRL_LEFT) {
      // move cursor left
      if(pos<=0) pos=strlen(s); //cycle
      else pos--;
      refresh = 1;
    } else if (key==KEY_CTRL_RIGHT) {
      // move cursor right
      if(pos>=(int)strlen(s)) pos=0; //cycle
      else pos++;
      refresh = 1;
    } else if ((c=dGetKeyChar(key))!=0) {
      char ns[2];
      ns[0] = c; ns[1]='\0';
      addStringToInput(s, ns, &pos, max, &refresh);
    }
    else if (key==KEY_CTRL_DEL) {
      if (pos<=0) continue;
      pos--;
      int i = pos;
      do {
              s[i] = s[i+1];
              i++;
      } while (s[i] != '\0');
      refresh = 1;
    }
    else if (key==KEY_CTRL_AC) {
      *s              = 0;
      pos             = 0;
      refresh = 1;
    }
    else if (key==KEY_CTRL_EXE) return 1;
    else if (key==KEY_CHAR_CR) return 5;
    else if (key!=KEY_CTRL_SHIFT && key!=KEY_CTRL_ALPHA && get_custom_key_handler_state()==1) {
      custom_key_to_handle = key; custom_key_to_handle_modifier = keyflag; return 3;
    }
    firstLoopRun = 0;
  }
  return 0;
}

int get_custom_fkey_label(int key);
void dConsoleRedraw() {
  Bdisp_AllClr_VRAM();
  drawFkeyLabels(get_custom_fkey_label(0), get_custom_fkey_label(1), get_custom_fkey_label(2), get_custom_fkey_label(3), 0x0307, get_custom_fkey_label(5)); // Catalog, LOAD, user-defined, user-defined,  A<>a, user-defined

  drawRectangle(0, 9*17+24, LCD_WIDTH_PX, 18, COLOR_WHITE);
  for(int i=0,j=line_start;i<line_count;++i) {
    locate(1,i+1);print((uchar*)line_buf[j]);
    if (++j>=LINE_ROW_MAX) j = 0;
  }
  DisplayStatusArea();
}
void dConsolePutChar (char c)
{
  if(outputRedirectBuffer != NULL && remainingBytesInRedirect) {
    *outputRedirectBuffer = c;
    outputRedirectBuffer++;
    *outputRedirectBuffer = '\0';
    remainingBytesInRedirect--;
    return; // mute real console
  }
  if (line_count == 0) line_count = 1;
  if (c != '\n') line_buf[line_index][line_x++] = c;
  else
  {
    line_buf[line_index][line_x] = '\0';
    line_x = LINE_COL_MAX;
  }
  if (line_x>=LINE_COL_MAX)
  {
    line_buf[line_index][line_x] = '\0';

    line_x = 0;
    if (line_count<LINE_ROW_MAX) ++line_count;
    else {
      line_start++;
      if (line_start>=LINE_ROW_MAX) line_start = 0;
    }
    line_index++;
    if (line_index>=LINE_ROW_MAX) line_index = 0;
  }
  line_buf[line_index][line_x] = '\0';
}

void dConsolePut(const char * str)
{
  if (line_count == 0) line_count = 1;
  for (;*str;++str) dConsolePutChar(*str);
  line_buf[line_index][line_x] = '\0';
}

void dPuts(const char * s)
{
  dConsolePut(s);
  dConsolePutChar('\n');
}

/*int dPrintf (const char * format,...)
{
  char    buf[256];
  int             length;
  va_list arg_list;

  va_start(arg_list,format);
  length = vsprintf(buf,format,arg_list);
  va_end(arg_list);

  dConsolePut (buf);
  
  return length;
}*/ 

void save_console_state_smem() {
  // ensure all timers are stopped and uninstalled before calling this function!
  int size = sizeof(line_row)*LINE_ROW_MAX + sizeof(int)*4;
  char buffer[sizeof(int)*4];

  memcpy(buffer, &line_index, sizeof(int));
  memcpy(buffer+4, &line_x, sizeof(int));
  memcpy(buffer+8, &line_start, sizeof(int));
  memcpy(buffer+12, &line_count, sizeof(int));

  unsigned short pFile[MAX_FILENAME_SIZE+1];
  // create file in data folder (assumes folder already exists)
  Bfile_StrToName_ncpy(pFile, (unsigned char*)CONSOLESTATEFILE, strlen(CONSOLESTATEFILE)+1);
  Bfile_CreateEntry_OS(pFile, CREATEMODE_FILE, &size);
  // if an error ocurrs when creating (because file already exists)
  // there's no need to delete and create again, because there's no problem
  // if there's junk at the end of the file.
  int hFile = Bfile_OpenFile_OS(pFile, READWRITE, 0); // Get handle
  if(hFile < 0) return;
  Bfile_WriteFile_OS(hFile, buffer, sizeof(buffer));
  Bfile_WriteFile_OS(hFile, line_buf, sizeof(line_row)*LINE_ROW_MAX);
  char cmdhist[N*INPUTBUFLEN+4] = "";
  get_cmd_history(cmdhist);
  int len = strlen(cmdhist);
  // make sure file ends with zeros:
  // (there can be junk at the end of the file):
  cmdhist[len] = 0;
  cmdhist[len+1] = 0;
  cmdhist[len+2] = 0;
  cmdhist[len+3] = 0;
  Bfile_WriteFile_OS(hFile, cmdhist, len+4);
  Bfile_CloseFile_OS(hFile);
}

void load_console_state_smem() {
  // ensure console memory has been initialized before calling this function!
  char filename[MAX_FILENAME_SIZE+1];
  sprintf(filename, DATAFOLDER"\\eigencon.erd"); // Eigenmath Restore Data
  unsigned short pFile[MAX_FILENAME_SIZE+1];
  Bfile_StrToName_ncpy(pFile, (unsigned char*)filename, strlen(filename)+1);
  int hFile = Bfile_OpenFile_OS(pFile, READWRITE, 0); // Get handle
  if(hFile < 0) {
    return;
  }
  char buffer[sizeof(int)*4];
  Bfile_ReadFile_OS(hFile, buffer, sizeof(buffer), 0);

  memcpy(&line_index, buffer, sizeof(int));
  memcpy(&line_x, buffer+4, sizeof(int));
  memcpy(&line_start, buffer+8, sizeof(int));
  memcpy(&line_count, buffer+12, sizeof(int));

  memset(line_buf, sizeof(line_row)*LINE_ROW_MAX, 0);
  Bfile_ReadFile_OS(hFile, line_buf, sizeof(line_row)*LINE_ROW_MAX, -1); // read console scrollback buffer

  // remaining contents (variable length) are the command history
  char cmdhist[N*INPUTBUFLEN] = "";
  Bfile_ReadFile_OS(hFile, cmdhist, N*INPUTBUFLEN, -1);

  unsigned char* src = (unsigned char*)cmdhist;
  unsigned char token[INPUTBUFLEN+1];
  src = toksplit(src, '\n' , token, INPUTBUFLEN);
  while (1) {
    update_cmd_history((char*)token);
    src = toksplit(src, '\n' , token, INPUTBUFLEN);
    if(!token[0]) break;
  }

  Bfile_CloseFile_OS(hFile);
  if(line_buf[line_index][0] == '\x1e') line_x = 0;
  else dConsolePutChar('\n');
}