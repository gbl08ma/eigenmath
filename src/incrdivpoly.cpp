#include "stdafx.h"

//-----------------------------------------------------------------------------
//
//	Divide polynomials
//
//	Input:		tos-4		Dividend
//
//			tos-3		Divisor
//
//			tos-2		Order
//
//			tos-1		x
//
//	Output:		tos-2		Quotient
//
//-----------------------------------------------------------------------------

#include "defs.h"


void
eval_incrdivpoly(void)
{
	/*int opemodesave;

	opemodesave = opemode;
	opemode = 1;*/

	push(cadr(p1));
	eval();

	push(caddr(p1));
	eval();

	push(cadddr(p1));
	eval();

	push(caddddr(p1));
	eval();

	incrdivpoly();
	
	//opemode = opemodesave;
}


void
incrdivpoly(void)
{
	save();
	yincrdivpoly();
	restore();
}


#define DIVIDEND p1
#define DIVISOR p2
#define ORDER p3
#define X p4
#define Q p5
#define QUOTIENT p6
#define TEMPO p7

void
yincrdivpoly(void)
{
	int h,i,j,m,n,x;
	U **dividend, **divisor;

	ORDER = pop();
	X = pop();
	DIVISOR = pop();
	DIVIDEND = pop();
	push(ORDER);
	x = pop_integer();

	h = tos;

	dividend = stack + tos;

	push(DIVIDEND);
	push(X);
	m = coeff() - 1;	// m is dividend's power

	divisor = stack + tos;
	
	push(DIVISOR);
	push(X);
	n = coeff() - 1;	// n is divisor's power

	TEMPO = alloc_tensor(n + m + x);
	TEMPO->u.tensor->ndim = 1;
	TEMPO->u.tensor->dim[0] = n + m + x;

	for(j=0; j<= n + m + x; j++) {
		push_integer(0);
		TEMPO->u.tensor->elem[j]=pop();
	}
	for(j=0; j<=m; j++) {
		TEMPO->u.tensor->elem[j]=dividend[j];
	}
	push(zero);

	for(j=0; j<= x; j++) {

		push(TEMPO->u.tensor->elem[j]);
		push(divisor[0]);
		divide();
		Q = pop();
		for (i = 0; i <= n; i++) {
			push(TEMPO->u.tensor->elem[j + i]);
			push(divisor[i]);
			push(Q);
			multiply();
			subtract();
			TEMPO->u.tensor->elem[j + i] = pop();
		}
		push(Q);
		push(X);
		push_integer(j);
		power();
		multiply();
		add();
	}
	QUOTIENT=pop();
	tos=h;
	push(QUOTIENT);
}
