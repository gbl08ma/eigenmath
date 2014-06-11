//-----------------------------------------------------------------------------
//
//	Author : philippe.billet@noos.fr
//
//	limit of f(x) x->a
//
//	Input:		tos-3		f
//
//			tos-2		x
//
//			tos-1		a
//
//	Output:		Result on stack
//
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "defs.h"

void
eval_limit(void)
{
	push(cadr(p1));
	eval();
	push(caddr(p1));
	eval();
	push(cadddr(p1));
	eval();
	limit();
}

void
limit(void)
{
	save();
	ylimit();
	restore();
}


#define F p1
#define X p2
#define Y p3
#define TEMP p4
#define DISP p5
#define taylorprec 5


void
ylimit(void)
{
	Y=pop();
	X=pop();
	F=pop();

	if (X == symbol(NIL) || Y == symbol(NIL))
		stop("limit formalism : limit(f,x,a)");
	if (find(Y,X))
		stop("limit : a shoudn't depend of x");

	if (equal(Y,symbol(INFTY)))
		limit_infty();
	else if (equal(Y,symbol(MINFTY)))
		limit_minfty();
	else limit_other();

}

void 
limit_infty(void)
{
 	//printstr("limit_infty\n");
	push(F);
	push(X);
	push(X);
	inverse();
/*	TEMP=pop();
	print(TEMP);
	printstr("\n");
	push(TEMP); */
	subst();
	eval();
/*	TEMP=pop();
	print(TEMP);
	printstr("\n");
	push(TEMP); */
	push(X);
	push_integer(0);
	limit();
	return;
}

void 
limit_minfty(void)
{
 	//printstr("limit_minfty\n");
	push(F);
	push(X);
	push(X);
	negate();
	inverse();
	subst();
	eval();
/*	TEMP=pop();
	print(TEMP);
	printstr("\n");
	push(TEMP); */
	push(X);
	push_integer(0);
	limit();
	return;
}



void 
limit_other(void)
{
 	//printstr("limit_other\n");
	push(F);
	denominator();
	TEMP=pop();
	if (!find(TEMP,X) || (car(F) == symbol(POWER) && 
			      isnegativeterm(cadr(F)))) {
//		printstr("result may be false, de l'Hospital rule not applicable \n");
		push(F);
		push(X);
		push(Y);
		subst();
		eval();
		return;
	}
	push(F);
	numerator();
	push(X);
	push_integer(taylorprec);
	push(Y);
	taylor();
	push(F);
	denominator();
	push(X);
	push_integer(taylorprec);
	push(Y);
	taylor();
	divide();
	inverse();
	inverse();
	TEMP=pop();
	push(TEMP);
	numerator();
	push(TEMP);
	denominator();
	push(X);
	push_integer(taylorprec);
	incrdivpoly();
	push(X);
	push(Y);
	subst();
	eval();
}
 
