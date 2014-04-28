// function for returning random numbers. added by gbl08ma
#include "stdafx.h"
#include "defs.h"

void
eval_random(void)
{
	randomnum();
}

static int rnd_seed;

void set_rnd_seed(int new_seed)
{
    rnd_seed = new_seed;
}

void randomnum(void)
{
  save();
  
  int k1;
  int ix = rnd_seed;

  k1 = ix / 127773;
  ix = 16807 * (ix - k1 * 127773) - k1 * 2836;
  if (ix < 0)
      ix += 2147483647;
  rnd_seed = ix;

  push_integer(rnd_seed);

  restore();
}