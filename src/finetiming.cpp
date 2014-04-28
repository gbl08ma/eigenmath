// function for returning fine timing numbers from the Prizm's TMU. added by gbl08ma
#include "stdafx.h"
#include "defs.h"

void
eval_finetiming(void)
{
	finetiming();
}

#define READ_ADDRESS(x) (*((unsigned int *)x))
void finetiming(void)
{
  save();
  unsigned int ft = READ_ADDRESS(0xA44D00D8);
  push_integer(ft);
  
  restore();
}