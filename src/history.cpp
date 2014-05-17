#include "stdafx.h"
#include "defs.h"
extern "C" {
#include "dConsole.h"
}
#include <stdlib.h>
#include <string.h>

extern char *get_curr_cmd(void);
extern void update_curr_cmd(char *);
//extern char *get_cmd_str(int);

#define N 21
#define HISTORYHEAP_N 25

static char *buf[N];
static int i, j, k;

char history_buf[HISTORYHEAP_N][1001];
char is_index_busy[HISTORYHEAP_N];

void initialize_history_heap() {
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
	char *d = history_malloc(/*strlen (s) + 1*/);
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

char *
get_cmd_history(void)
{
	int k, n;
	char *s, *t;

	// measure

	n = 0;
	k = j;
	while (k != i) {
		n += (int) strlen(buf[k]) + 2;
		k = (k + 1) % N;
	}

	s = (char *) history_malloc(/*n + 1*/);

	if (s == NULL)
		return NULL;

	// copy

	t = s;
	k = j;
	while (k != i) {
		strcpy(t, buf[k]);
		k = (k + 1) % N;
		t += strlen(t);
		*t++ = '\r';
		*t++ = '\n';
	}

	*t = 0;

	return s;
}