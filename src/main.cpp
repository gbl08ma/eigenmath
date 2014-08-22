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

#include "defs.h"
extern "C" {
#include "dConsole.h"
#include "memmgr.h"
}
#include "fileGUI.hpp"
#include "textGUI.hpp"
#include "graphicsProvider.hpp"
#include "constantsProvider.hpp"

extern int esc_flag;
extern int run_startup_script_again;
extern void set_rnd_seed(int);
typedef char history_line[INPUTBUFLEN+1];
extern void initialize_history_heap(history_line* area);
int execution_in_progress = 0;
int custom_key_to_handle;
int custom_key_to_handle_modifier;
int has_drawn_graph = 0;
static char expr[INPUTBUFLEN];

void check_do_graph();
int run_script(char* filename);
void run_startup_script();
void save_session();
int restore_session();
void check_execution_abort();
void select_script_and_run();
void select_strip_script();
void script_recorder();
void dump_eigenmath_symbols_smem();
void input_eval_loop(int isRecording);
int is_running_in_strip();
static int aborttimer = 0;
extern int eigenmathRanAtLeastOnce;

extern char* outputRedirectBuffer;
extern int remainingBytesInRedirect;
int
main()
{
  line_row line_buf[LINE_ROW_MAX];
  initializeConsoleMemory(line_buf);
  memmgr_init(); // initialize special heap for atoms (alloc.cpp)
  history_line history_buf[HISTORYHEAP_N]; // so this goes on the stack and not on the static ram
  initialize_history_heap(history_buf);
  Bdisp_AllClr_VRAM();
  Bdisp_EnableColor(1);
  EnableStatusArea(0);
  DefineStatusAreaFlags(3, SAF_BATTERY | SAF_TEXT | SAF_GLYPH | SAF_ALPHA_SHIFT, 0, 0);
  // disable Catalog function throughout the add-in, as we don't know how to make use of it:
  Bkey_SetAllFlags(0x80);
  SetQuitHandler(save_session); // automatically save session when exiting
  set_rnd_seed(RTC_GetTicks());
  puts("Welcome to Eigenmath\nTo see more options, press\nShift then Menu.");
  aborttimer = Timer_Install(0, check_execution_abort, 100);
  if (aborttimer > 0) { Timer_Start(aborttimer); }
  //in case we're running in a strip, check if this strip has a script to run.
  if(is_running_in_strip()) {
    run_startup_script();
    int MCSsize;
    MCSGetDlen2(DIRNAME, SCRIPTFILE, &MCSsize);
    if (MCSsize > 0) {
      // there is a script to run...
      unsigned char* asrc = (unsigned char*)alloca(MCSsize*sizeof(unsigned char)+5); // 5 more bytes to make sure it fits...
      MCSGetData1(0, MCSsize, asrc); // read script from MCS
      execution_in_progress = 1;
      run((char*)asrc);
      eigenmathRanAtLeastOnce = 1;
      execution_in_progress = 0;
    }
  } else {
    // not in strip, restore last session
    if(!restore_session()) run_startup_script();
  }
  run_startup_script_again = 0;
  input_eval_loop(0);
}
static int exproffset = 0;
void input_eval_loop(int isRecording) {
  char** recHistory = NULL; int curRecHistEntry = 0;
  if(isRecording) recHistory = (char**)alloca(200); // space for 200 pointers to history entries
  exproffset = 0;
  int nodel = 0;
  while (1) {
    DefineStatusMessage((char*)(isRecording ? "Recording ('record' to stop)" : ""), 1, 0, 0);
    has_drawn_graph = 0;
    if(!nodel) strcpy(expr+exproffset, (char*)"");
    else nodel = 0;
    dConsolePutChar((exproffset ? 147 : '\x1e'));
    int res = gets(expr+exproffset,INPUTBUFLEN-exproffset, isRecording, !!exproffset); // isRecording is provided for UI changes, no behavior changes.
    if(res == 2) {
      exproffset = 0;
      dConsolePutChar('\n');
      select_script_and_run();
      nodel = 1;
      continue;
    } else if(res == 3) {
      dConsolePutChar('\n');
      char buf[100];
      sprintf(buf, "prizmUIkeyHandler(%d,%d)", custom_key_to_handle, custom_key_to_handle_modifier);
      execution_in_progress = 1;
      run(buf);
      eigenmathRanAtLeastOnce = 1;
      execution_in_progress = 0;
      check_do_graph();
      if(run_startup_script_again) { run_startup_script_again = 0; run_startup_script(); }
      nodel = 1;
      continue;
    } else if(res == 4) {
      exproffset = 0;
      dConsolePutChar('\n');
      select_strip_script();
      nodel = 1;
      continue;
    } else if(res == 6) {
      // AC partial input
      exproffset = 0;
      dConsolePutChar('\n');
      continue;
    }
    puts(expr+exproffset);
    update_cmd_history(expr+exproffset);
    if(res == 5) {
      // partial command entered.
      exproffset += strlen(expr+exproffset);
      continue;
    }
    exproffset = 0;
#ifdef ENABLE_DEBUG
    if(strcmp(expr, "testmode") == 0) {
      TestMode(1);
    } else if(strcmp(expr, "meminfo") == 0) {
      print_mem_info();
    } else if(strcmp(expr, "memgc") == 0) {
      gc();
    } else
#endif
    if(strcmp(expr, "record") == 0) {
      if(!isRecording) script_recorder();
      else {
        // create and save a script. this must be done here, because we used alloca
        // the "clean" way would be using malloc&free, but on the Prizm the heap is already being heavily used by the Eigenmath core.
        if(curRecHistEntry == 0) {
          puts("Nothing to record.");
          return;
        }
        dConsolePut("Recording stopped.\nType a name for the script, or\nleave empty to discard.\n:");
        char inputname[MAX_FILENAME_SIZE+1];
        inputname[0] = 0;
        gets(inputname,MAX_FILENAME_SIZE-50);
        puts(inputname);
        if(!strlen(inputname)) {
          // user aborted
          puts("Recording discarded.");
          return;
        }
        if (aborttimer > 0) {
          Timer_Stop(aborttimer);
          Timer_Deinstall(aborttimer);
        }
        char filename[MAX_FILENAME_SIZE+1];
        sprintf(filename, "\\\\fls0\\%s.txt", inputname);
        unsigned short pFile[MAX_FILENAME_SIZE+1];
        Bfile_StrToName_ncpy(pFile, (unsigned char*)filename, strlen(filename)+1);
        // calculate size
        int size = 0;
        int maxHistory = curRecHistEntry - 1; //because we ++'ed at the end of last addition
        for(int i=0; i <= maxHistory; i++) {
          size = size + strlen(recHistory[i]) + 1; // 1 byte for \n. we will use unix line termination
        }
        int BCEres = Bfile_CreateEntry_OS(pFile, CREATEMODE_FILE, &size);
        if(BCEres >= 0) // Did it create?
        {
          BCEres = Bfile_OpenFile_OS(pFile, READWRITE, 0); // Get handle
          if(BCEres >= 0) {
            for(int i=0; i <= maxHistory; i++) {
              char buf[strlen(recHistory[i])+5];
              strcpy(buf, recHistory[i]);
              strcat(buf, (char*)"\n");
              Bfile_WriteFile_OS(BCEres, buf, strlen(recHistory[i])+1);
            }
            Bfile_CloseFile_OS(BCEres);
            puts("Script created.");
          } else {
            puts("An error occurred when creating the script for recording.");
          }
        } else {
          puts("An error occurred when creating the script for recording.");
        }
        aborttimer = Timer_Install(0, check_execution_abort, 100);
        if (aborttimer > 0) Timer_Start(aborttimer);
        return;
      }
    } else {
      execution_in_progress = 1;
      has_drawn_graph = 0;
      run(expr);
      eigenmathRanAtLeastOnce = 1;
      // run_startup_script cannot run from inside eval_clear because then it would be a run() inside a run()
      if(run_startup_script_again) { run_startup_script_again = 0; run_startup_script(); }
      execution_in_progress = 0;
      
      // if recording, add input to record
      if(isRecording && curRecHistEntry < 200) {
        recHistory[curRecHistEntry] = (char*)alloca(strlen(expr)+2); // 2 bytes for security
        strcpy(recHistory[curRecHistEntry], expr);
        curRecHistEntry++;
      }
      check_do_graph();
    }
  }
}

void check_do_graph() {
  if(has_drawn_graph) {
    has_drawn_graph = 0;
    int key;
    int fkeymenu = 0;
    while(1) {
      DisplayStatusArea();
      if(fkeymenu == 1) drawFkeyLabels(0x005F, 0x0060, 0x0061); // INITIAL, TRIG, STANDRD
      GetKey(&key);
      double xmin, xmax, ymin, ymax, xrange, yrange;
      get_xyminmax(&xmin, &xmax, &ymin, &ymax);
      xrange = xmax - xmin;
      yrange = ymax - ymin;
      if(fkeymenu == 0) {
        if(key == KEY_CTRL_LEFT || key == KEY_CTRL_RIGHT) {
          if(key==KEY_CTRL_LEFT) {
            xmin -= xrange * 0.15;
            xmax -= xrange * 0.15;
          } else {
            xmin += xrange * 0.15;
            xmax += xrange * 0.15;
          }
          // easier than having to set the symbols in the complicated eigenmath system:
          char command[100];
          sprintf(command, "xrange=(%g,%g)", xmin, xmax);
          execution_in_progress = 1;
          has_drawn_graph = 0;
          run(command);
          run(expr);
          execution_in_progress = 0;
        } else if(key == KEY_CTRL_DOWN || key == KEY_CTRL_UP) {
          if(key==KEY_CTRL_DOWN) {
            ymin -= yrange * 0.15;
            ymax -= yrange * 0.15;
          } else {
            ymin += yrange * 0.15;
            ymax += yrange * 0.15;
          }
          // easier than having to set the symbols in the complicated eigenmath system:
          char command[100];
          sprintf(command, "yrange=(%g,%g)", ymin, ymax);
          execution_in_progress = 1;
          has_drawn_graph = 0;
          run(command);
          run(expr);
          execution_in_progress = 0;
        } else if(key == KEY_CHAR_PLUS || key == KEY_CHAR_MINUS) {
          if(key==KEY_CHAR_PLUS) {
            // 0.75 is 3/4
            xmin = xmin * 0.75;
            xmax = xmax * 0.75;
            ymin = ymin * 0.75;
            ymax = ymax * 0.75;
          } else {
            // 1.(3), or 1/(3/4), or 4/3
            xmin = xmin * 4.0/3.0;
            xmax = xmax * 4.0/3.0;
            ymin = ymin * 4.0/3.0;
            ymax = ymax * 4.0/3.0;
          }
          // easier than having to set the symbols in the complicated eigenmath system:
          char command[100];
          sprintf(command, "yrange=(%g,%g)", ymin, ymax);
          execution_in_progress = 1;
          run(command);
          sprintf(command, "xrange=(%g,%g)", xmin, xmax);
          has_drawn_graph = 0;
          run(command);
          run(expr);
          execution_in_progress = 0;
        } else if(key == KEY_CTRL_F3) {
          fkeymenu = 1;
          key = 0;
        } else if(key == KEY_CTRL_EXIT || key==KEY_CTRL_F6) return;
      }
      if(fkeymenu == 1) {
        if(key == KEY_CTRL_F1) {
          execution_in_progress = 1;
          run("xrange=(-10,10)");
          run("yrange=(-10,10)");
          key = KEY_CTRL_EXIT; // redraw and close menu
        } else if(key == KEY_CTRL_F2) {
          execution_in_progress = 1;
          char command[100];
          sprintf(command, "xrange=(%g,%g)", -3.0*M_PI, 3.0*M_PI);
          run(command);
          run("yrange=(-1.6, 1.6)");
          key = KEY_CTRL_EXIT; // redraw and close menu
        } else if(key == KEY_CTRL_F3) {
          execution_in_progress = 1;
          run("xrange=(-10,10)");
          run("yrange=(-5,5)");
          key = KEY_CTRL_EXIT; // redraw and close menu
        }
        if(key == KEY_CTRL_EXIT) { // must not be a "else if"
          fkeymenu = 0;
          execution_in_progress = 1;
          has_drawn_graph = 0;
          run(expr);
          execution_in_progress = 0;
        }
      }
    }
  }
}

int run_script(char* filename) {
  // returns 1 if script was run, 0 otherwise
  unsigned short pFile[MAX_FILENAME_SIZE+1];
  Bfile_StrToName_ncpy(pFile, (unsigned char*)filename, strlen(filename)+1); 
  int hFile = Bfile_OpenFile_OS(pFile, READWRITE, 0); // Get handle
  
  // Check for file existence
  if(hFile >= 0) // Check if it opened
  {
    // Returned no error, file exists, open it
    int size = Bfile_GetFileSize_OS(hFile);
    // File exists and has size 'size'
    // Read file into a buffer
    if ((unsigned int)size > MAX_TEXTVIEWER_FILESIZE) {
      Bfile_CloseFile_OS(hFile);
      puts("Stop: script too big");
      return 0; //file too big, return
    }
    unsigned char* asrc = (unsigned char*)alloca(size*sizeof(unsigned char)+5); // 5 more bytes to make sure it fits...
    memset(asrc, 0, size+5); //alloca does not clear the allocated space. Make sure the string is null-terminated this way.
    int rsize = Bfile_ReadFile_OS(hFile, asrc, size, 0);
    Bfile_CloseFile_OS(hFile); //we got file contents, close it
    asrc[rsize]='\0';
    execution_in_progress = 1;
    run((char*)asrc);
    eigenmathRanAtLeastOnce = 1;
    execution_in_progress = 0;
    return 1;
  }
  return 0;
}
void run_startup_script() {
  run_script(DATAFOLDER"\\eigensup.txt");
}

int get_set_session_setting(int value) {
  // value is -1 to get only
  // 0 to disable sesison save/load, 1 to enable
  // if MCS file is present, disable session load/restore
  if(value == -1) {
    int size;
    MCSGetDlen2(DIRNAME, (unsigned char*)SESSIONFILE, &size);
    if (!size) return 1;
    else return 0;
  } else if(value == 1) {
    MCSDelVar2(DIRNAME, (unsigned char*)SESSIONFILE);
    return 1;
  } else if(value == 0) {
    MCS_CreateDirectory(DIRNAME);
    unsigned char buffer[2];
    buffer[0] = 1;
    buffer[1] = 1;
    MCSPutVar2(DIRNAME, (unsigned char*)SESSIONFILE, 2, buffer);
    return 0;
  }
  return -1;
}

void create_data_folder() {
  unsigned short pFile[MAX_FILENAME_SIZE+1];
  Bfile_StrToName_ncpy(pFile, (unsigned char*)DATAFOLDER, strlen(DATAFOLDER)+1);
  Bfile_CreateEntry_OS(pFile, CREATEMODE_FOLDER, 0);
}

void save_session() {
  if(!get_set_session_setting(-1)) return;
  if(!eigenmathRanAtLeastOnce || is_running_in_strip()) return;
  if (aborttimer > 0) {
    Timer_Stop(aborttimer);
    Timer_Deinstall(aborttimer);
  }
  save_console_state_smem(); // call before dump_eigenmath_symbols_smem(), because this calls create_data_folder if necessary!
  dump_eigenmath_symbols_smem();

  // this is only called on exit, no need to reinstall the check_execution_abort timer.
}
int restore_session() {
  // 1 if session was restored, 0 otherwise
  if(!get_set_session_setting(-1)) return 0;
  int r = run_script(SYMBOLSSTATEFILE);
  load_console_state_smem();
  return r;
}
void select_script_and_run() {
  char filename[MAX_FILENAME_SIZE+1];
  if(fileBrowser(filename, (unsigned char*)"*.txt", "Scripts")) {
    run_script(filename);
  }
}

void select_strip_script() {  
  textArea text;

  textElement elem[4];
  text.elements = elem;
  
  elem[0].text = (char*)"This function lets you run a script when this eActivity strip is opened.";
  elem[1].newLine = 1;
  elem[1].text = (char*)"When sharing the eActivity file, it will not be necessary to share any other files - the script is included in the eActivity.";
  elem[2].newLine = 1;
  elem[2].text = (char*)"You will be informed if the script is too big to fit inside the eActivity file.";
  elem[3].newLine = 1;
  elem[3].text = (char*)"Press EXIT now to continue and select a script.";
  text.numelements = 4;
  text.scrollbar = 0;
  doTextArea(&text);
  
  char filename[MAX_FILENAME_SIZE+1];
  if(fileBrowser(filename, (unsigned char*)"*.txt", "Scripts")) {
    // get the size of the selected script on SMEM.
    // get free size on the "MCS" of the strip we're running on and see if the script fits
    unsigned short pFile[MAX_FILENAME_SIZE+1];
    Bfile_StrToName_ncpy(pFile, (unsigned char*)filename, strlen(filename)+1); 
    int hFile = Bfile_OpenFile_OS(pFile, READWRITE, 0); // Get handle
    if(hFile >= 0) // Check if it opened
    { //opened
      unsigned int filesize = Bfile_GetFileSize_OS(hFile);  
      //get free size of MCS
      int MCSmaxspace; int MCScurrentload; int MCSfreespace;  
      MCS_GetState( &MCSmaxspace, &MCScurrentload, &MCSfreespace );
      if((int)filesize < MCSfreespace - 50) { // 50 bytes for any headers and the like
        // fits, copy selected script to MCS
        unsigned char* scontents = (unsigned char*)alloca(filesize);
        int rsize = Bfile_ReadFile_OS(hFile, scontents, filesize, 0);
        scontents[rsize]='\0';
        // script is now in buffer scontents
        // write it to the "MCS"
        int createResult = MCS_CreateDirectory( DIRNAME );
        if(createResult != 0) // Check directory existence
        { // directory already exists, so delete the exiting file that may be there
          MCSDelVar2(DIRNAME, SCRIPTFILE);
        }
        MCSPutVar2(DIRNAME, SCRIPTFILE, rsize, scontents);
        puts("Script set successfully.");
      } else {
        puts("The script is too big to be\nincluded in the eActivity.");
      }
      Bfile_CloseFile_OS(hFile);
      return; // don't get to the error message
    }
  }
  puts("There was a problem setting the script for this strip.");
}

void script_recorder() {
  puts("Recording started: every\ncommand you enter from now on\nwill be recorded, so that you\ncan create a script.\nWhen you're done recording,\ncall \"record\" again.");
  input_eval_loop(1);
}

void dump_eigenmath_symbols_smem() {
  // ensure all timers are stopped and uninstalled before calling this function!
  unsigned short pFile[MAX_FILENAME_SIZE+1];
  // create file in data folder (assumes data folder already exists)
  Bfile_StrToName_ncpy(pFile, (unsigned char*)SYMBOLSSTATEFILE, strlen(SYMBOLSSTATEFILE)+1);
  // even if it already exists, there's no problem,
  // in the event that our file shrinks, we just let junk be at the end of
  // the file (after two null bytes, of course).
  int hFile = Bfile_OpenFile_OS(pFile, READWRITE, 0); // Get handle
  if(hFile < 0) {
    // error. file does not exist yet. try creating it
    // (data folder should exist already, as save_console_state_smem() should have been called before this function)
    int size = 1;
    Bfile_CreateEntry_OS(pFile, CREATEMODE_FILE, &size);
    // now try opening
    hFile = Bfile_OpenFile_OS(pFile, READWRITE, 0); // Get handle
    if(hFile < 0) return; // if it still fails, there's nothing we can do
  }
  char buffer[5000];
  int lb = 0;
  for (int i = AUTOEXPAND; i < NSYM; i++) { // dump all symbols after AUTOEXPAND
    if((i >= YYE && i <= SECRETX) || (i >= C1 && i <= C6)) continue; // do not dump special-purpose internal symbols
    if (symtab[i].u.printname == 0)
      break;
    char symval[2000];
    symval[0]=0;
    outputRedirectBuffer = symval;
    remainingBytesInRedirect = 2000;
    printline(get_binding(symbol(i)));
    int svl = 2000 - remainingBytesInRedirect;
    if(svl) symval[svl-1] = 0; // remove \n at end to make comparison with printname possible
    char symarg[1000];
    outputRedirectBuffer = symarg;
    remainingBytesInRedirect = 1000;
    print_arg_list(get_arglist(symbol(i)));
    outputRedirectBuffer = NULL;
    // avoid saving abc=abc, to avoid symbol table exhaustion
    // in this situation, remainingBytesInRedirect==998 has the same effect as !strcmp(symarg,"()"),
    // because if symarg is only two chars long, then they must be the ().
    if(remainingBytesInRedirect==998 && !strcmp(symval, symtab[i].u.printname)) continue;

    lb += sprintf(buffer+lb, "%s%s=%s\n", symtab[i].u.printname, symarg, symval);
    if(lb > 2000) { // are there enough contents in the buffer to issue a write?
      Bfile_WriteFile_OS(hFile, buffer, lb);
      buffer[0]=0;
      lb = 0;
    }
  }
  // make sure it is null-terminated:
  // (there can be junk at the end of the file, because we're not recreating it clean every time)
  buffer[lb] = 0;
  buffer[lb+1] = 0;
  // write what hasn't been written yet
  Bfile_WriteFile_OS(hFile, buffer, strlen(buffer)+2); //make sure to write the two zeros
  Bfile_CloseFile_OS(hFile);
  // done
}

void check_execution_abort() {
  if(execution_in_progress) {
    HourGlass();
    short unsigned int key = PRGM_GetKey();
    if(key == KEY_PRGM_ACON) esc_flag = 1;
  }
}
int get_custom_key_handler_state() {
  U* tmp = usr_symbol("prizmUIhandleKeys");
  if (!issymbol(tmp)) return 0;
  tmp = get_binding(tmp);
  if(isnonnegativeinteger(tmp)) {
    return !iszero(tmp);
  } else return 0;
}
int get_custom_fkey_label(int fkey) {
  U* tmp;
  if(fkey==0) {
    tmp = usr_symbol("prizmUIfkey1label");
  } else if(fkey==1) {
    tmp = usr_symbol("prizmUIfkey2label");
  } else if(fkey==2) {
    tmp = usr_symbol("prizmUIfkey3label");
  } else if (fkey==3) {
    tmp = usr_symbol("prizmUIfkey4label");
  } else if (fkey==5) {
    tmp = usr_symbol("prizmUIfkey6label");
  } else return 0;
  if (issymbol(tmp)) {
    tmp = get_binding(tmp);
    if(isnonnegativeinteger(tmp)) {
      return *tmp->u.q.a;
    }
  }
  return 0;
}

int is_running_in_strip() {
  static int had_determined = -1;
  if(had_determined != -1) return had_determined;
  /* Simon Lothar says:
   ** If I run INSIGHT from out of an EACT-strip, there are three variables (EACT1, EDIT_PAC and PACKDUMY)
   ** in the main memory directory @EACT.
   ** If I run INSIGHT from out of the MAIN MENU, the main memory directory @EACT is empty.
   * This code checks the presence of EACT1 to detect whether the add-in is running as a strip.
   * Checking whether the first pixel of the status area is green would also work, but I think this is a
   * cleaner solution.
   */
  int size;
  MCSGetDlen2((unsigned char*)"@EACT", (unsigned char*)"EACT1", &size);
  had_determined = !!size;
  return !!size;
}

void
clear_term()
{
  // user issued "clear" command.
  dConsoleCls();
}

extern void eval_print(void);

void
eval_display(void)
{
  eval_print();
}

void
printstr(char *s)
{
  while (*s)
    printchar(*s++);
}

extern int test_flag;

void
printchar(int c)
{
  dConsolePutChar(c);
}

void
printchar_nowrap(int c)
{
  printchar(c);
}

void
eval_sample(void)
{
}

// Command history related:
extern char* history_malloc();
char *
get_curr_cmd(void)
{
  int i, len;
  char *s;

  len=strlen(expr+exproffset);
  s = (char*)history_malloc();
  strcpy(s, expr+exproffset);

  // trim trailing spaces

  for (i = len - 1; i >= 0; i--) {
    if (isspace(s[i])) s[i] = 0;
    else break;
  }

  return s;
}
void
update_curr_cmd(char *s)
{
  strncpy(expr+exproffset, s, INPUTBUFLEN-exproffset);
}