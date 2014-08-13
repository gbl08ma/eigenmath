#include "stdafx.h"
#include "defs.h"
extern "C" {
#include "dConsole.h"
}
#include <stdlib.h>
#include <string.h>

extern char *get_curr_cmd(void);
extern void update_curr_cmd(char *);

static char *buf[N];
static int i, j, k;

typedef char history_line[INPUTBUFLEN+1];
history_line *history_buf;
char is_index_busy[HISTORYHEAP_N];

void initialize_history_heap(history_line* area) {
	history_buf = area;
	for(int k = 0; k<HISTORYHEAP_N; k++) {
		is_index_busy[k] = 0;
	}
}

char* history_malloc() {
  for(int k = 0; k<HISTORYHEAP_N; k++) {
    if(is_index_busy[k] == 0) {
      is_index_busy[k] = 1;
      strcpy(history_buf[k], (char*)"");
      char bogus[20];
      sprintf(bogus, "m%i;", k);
      return history_buf[k];
    }
  }
  return NULL;
}

void history_free(char* index_addr) {
  for(int k = 0; k<HISTORYHEAP_N; k++) {
    if(history_buf[k] == index_addr) {
      is_index_busy[k] = 0;
      char bogus[20];
      sprintf(bogus, "f%i;", k);
      return;
    }
  }
  // oops...
}

char* history_strdup(const char* s) {
	char *d = history_malloc();
  if (d == NULL) return NULL;
  strcpy (d,s);
  char bogus[1005];
  sprintf(bogus, "d: %s", d);
  return d;
}

void
update_cmd_history(char *s)
{
	// reset history pointer

	k = i;

	// blank string?

	if (*s == 0)
		return;

	// no duplicates

	if (i != j && strcmp(s, buf[(i + N - 1) % N]) == 0)
		return;

	buf[i] = history_strdup(s);

	i = (i + 1) % N;

	if (i == j) {
		history_free(buf[j]);
		buf[j] = 0;
		j = (j + 1) % N;
	}

	k = i;
}

// k != i indicates curr cmd is historical

void
do_up_arrow(void)
{
	char *s;

	// save curr cmd if new input or change to historical input

	s = get_curr_cmd();

	if (*s) {
		if (k == i || strcmp(s, buf[k]) != 0) {
			update_cmd_history(s);
			k = (i + N - 1) % N;
		}
	}

	history_free(s);

	// retard history pointer

	if (k != j)
		k = (k + N - 1) % N;

	if (buf[k])
		update_curr_cmd(buf[k]);
	else
		update_curr_cmd("");
}

void
do_down_arrow(void)
{
	char *s;

	// save curr cmd if new input or change to historical input

	s = get_curr_cmd();

	if (*s) {
		if (k == i || strcmp(s, buf[k]) != 0) {
			update_cmd_history(s);
			k = (i + N - 1) % N;
		}
	}

	history_free(s);

	// advance history pointer

	if (k != i)
		k = (k + 1) % N;

	if (buf[k])
		update_curr_cmd(buf[k]);
	else
		update_curr_cmd("");
}

void get_cmd_history(char* out) {
	*out=0;
	int k = j;
	while (k != i) {
		strcat(out, buf[k]);
		strcat(out, (char*)"\n");
		k = (k + 1) % N;
	}
}