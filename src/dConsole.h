#ifndef DCONSOLE_H
#define DCONSOLE_H
#define INPUTBUFLEN 500
#define LINE_ROW_MAX    /*10*/ 200
#define LINE_COL_MAX    32
//command history numbers:
#define N 41
#define HISTORYHEAP_N N+4
typedef char line_row[LINE_COL_MAX+1];

extern int     vsprintf(char *, const char *, char *);
typedef char *va_list ;

#define va_start(ap,param) (void)((ap)=(int)\
((char *) &(param)+sizeof(param))%4u?\
(char *) &(param)+sizeof(param)+(4u-(sizeof(param)%4u)):\
(char *) &(param)+sizeof(param))

#define va_arg(ap,type) (*((ap)=((int)((ap)+sizeof(type))%4u?\
(ap)+sizeof(type)+(4u-(sizeof(type)%4u)):\
(ap)+sizeof(type)),\
(type *)((int)((ap)-sizeof(type))%4u?\
(ap)-sizeof(type)-(4u-(sizeof(type)%4u)):\
(ap)-sizeof(type))))

#define va_end(ap)

void initializeConsoleMemory(line_row* area);

int dGetLineBox (char * s,int max,int width,int x,int y);

void dConsoleRedraw ();

void dConsolePut(const char * str);
void dConsolePutChar (char c);

int dGetLine (char * s,int max, int isRecording=0);

int dPrintf (const char * format,...);

void dConsoleCls ();
void dPuts(const char *);

#define printf          dPrintf
#define puts            dPuts
#define putchar         dConsolePutChar
#define gets            dGetLine

void save_console_state_smem();
void load_console_state_smem();

#endif