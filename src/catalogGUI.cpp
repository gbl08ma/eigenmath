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

#include "menuGUI.hpp"
#include "catalogGUI.hpp"
#include "graphicsProvider.hpp"
#include "textGUI.hpp"

const catalogFunc completeCat[CAT_COMPLETE_COUNT] = { // list of all functions (including some not in any category)
  {"abs(x)", "abs(", "Returns the absolute value or vector length of x. The mag function should be used for complex x.", CAT_CATEGORY_LINEARALG},
  {"adj(m)", "adj(", "Returns the adjunct of matrix m.", CAT_CATEGORY_LINEARALG},
  {"and(a,b,...)", "and(", "Returns the logical \"and\" of predicate expressions.", CAT_CATEGORY_PROG},
  {"arccos(x)", "arccos(", "Returns the inverse cosine of x.", CAT_CATEGORY_TRIG},
  {"arccosh(x)", "arccosh(", "Returns the inverse hyperbolic cosine of x.", CAT_CATEGORY_TRIG},
  {"arcsin(x)", "arcsin(", "Returns the inverse sine of x.", CAT_CATEGORY_TRIG},
  {"arcsinh(x)", "arcsinh(", "Returns the inverse hyperbolic sine of x.", CAT_CATEGORY_TRIG},
  {"arctan(x)", "arctan(", "Returns the inverse tangent of x.", CAT_CATEGORY_TRIG},
  {"arctanh(x)", "arctanh(", "Returns the inverse hyperbolic tangent of x.", CAT_CATEGORY_TRIG},
  {"arg(z)", "arg(", "Returns the angle of complex z.", CAT_CATEGORY_COMPLEXNUM},
  {"besselj(x,n)", "besselj(", "Evaluates the Bessel function of the first kind at value x and order n.", CAT_CATEGORY_SPECIAL},
  {"binding(x)", "binding(", "Returns the unevaluated binding of a symbol.", CAT_CATEGORY_OTHER},
  {"binomial(n,k)", "binomial(", "Same as choose(n,k).", CAT_CATEGORY_POLYNOMIAL},
  {"ceiling(x)", "ceiling(", "Returns the smallest integer not less than x.", CAT_CATEGORY_OTHER},
  {"check(x)", "check(", "In a script, if the predicate x is true then continue, else stop.", CAT_CATEGORY_PROG},
  {"choose(n,k)", "choose(", "Returns nCk. For example, the number of five card hands is choose(52,5).", CAT_CATEGORY_OTHER},
  {"circexp(x)", "circexp(", "Returns expression x with circular functions converted to exponential forms. Sometimes this will simplify an expression.", CAT_CATEGORY_CALCULUS},
  {"clock(z)", "clock(", "Returns complex z in clock form.", CAT_CATEGORY_COMPLEXNUM},
  {"coeff(p,x,n)", "coeff(", "Returns the coefficient of x^n in polynomial p.", CAT_CATEGORY_LINEARALG},
  {"cofactor(m,i,j)", "cofactor(", "Returns of the cofactor of matrix m with respect to row i and column j.", CAT_CATEGORY_LINEARALG},
  {"condense(x)", "condense(", "Condenses expression x by factoring common terms.", CAT_CATEGORY_OTHER},
  {"conj(z)", "conj(", "Returns the complex conjugate of z.", CAT_CATEGORY_COMPLEXNUM},
  {"contract(a,i,j)", "contract(", "Returns tensor a summed over indices i and j. If i and j are omitted then indices 1 and 2 are used. contract(m) is equivalent to the trace of matrix m.", CAT_CATEGORY_LINEARALG},
  {"cos(x)", "cos(", "Returns the cosine of x.", CAT_CATEGORY_TRIG},
  {"cosh(x)", "cosh(", "Returns the hyperbolic cosine of x.", CAT_CATEGORY_TRIG},
  {"cross(u,v)", "cross(", "Returns the cross product of vectors u and v.", CAT_CATEGORY_LINEARALG},
  {"curl(u)", "curl(", "Returns the curl of vector u.", CAT_CATEGORY_LINEARALG},
  {"d(f,x)", "d(", "Returns the derivative of f with respect to x.", CAT_CATEGORY_CALCULUS},
  {"defint(f,x,a,b,...)", "defint(", "Returns the definite integral of f with respect to x evaluated from a to b. The argument list can be extended for multiple integrals. For example, d(f,x,a,b,y,c,d).", CAT_CATEGORY_CALCULUS},
  {"deg(p,x)", "deg(", "Returns the degree of polynomial p in x.", CAT_CATEGORY_POLYNOMIAL},
  {"denominator(x)", "denominator(", "Returns the denominator of expression x.", CAT_CATEGORY_OTHER},
  {"det(m)", "det(", "Returns the determinant of matrix m.", CAT_CATEGORY_LINEARALG},
  {"dim(a,n)", "dim(", "Returns dimension of tensor a.", CAT_CATEGORY_LINEARALG},
  {"dirac(x)", "dirac(", "Dirac function. dirac(-x)=dirac(x), dirac(b-a)=dirac(a-b).", CAT_CATEGORY_OTHER},
  {"divisors(x)", "divisors(", "Returns the divisors of expression x.", CAT_CATEGORY_CALCULUS},
  {"do(a,b,...)", "do(", "Evaluates the argument list from left to right. Returns the result of the last argument.", CAT_CATEGORY_PROG},
  {"dot(a,b,...)", "dot(", "Returns the dot product of tensors.", CAT_CATEGORY_LINEARALG},
  {"draw(f,x)", "draw(", "Draws the function f with respect to x.", CAT_CATEGORY_OTHER},
  {"eigen(m)", "eigen(", "Stores eigenvalues of matrix m in diagonal matrix D and eigenvectors in matrix Q. D and Q are actual symbol names.", CAT_CATEGORY_LINEARALG},
  {"eigenval(m)", "eigenval(", "Returns eigenvalues of matrix m.", CAT_CATEGORY_LINEARALG},
  {"eigenvec(m)", "eigenvec(", "Returns eigenvectors of matrix m.", CAT_CATEGORY_LINEARALG},
  {"erf(x)", "erf(", "Returns the error function of x.", CAT_CATEGORY_OTHER},
  {"erfc(x)", "erfc(", "Returns the complementary error function of x.", CAT_CATEGORY_OTHER},
  {"eval(f,x,n)", "eval(", "Returns f evaluated at x = n.", CAT_CATEGORY_CALCULUS},
  {"exp(x)", "exp(", "Returns e^x.", CAT_CATEGORY_OTHER},
  {"expand(r,x)", "expand(", "Returns the partial fraction expansion of the ratio of polynomials r in x.", CAT_CATEGORY_POLYNOMIAL},
  {"expcos(x)", "expcos(", "Returns the cosine of x in exponential form.", CAT_CATEGORY_TRIG},
  {"expsin(x)", "expsin(", "Returns the sine of x in exponential form.", CAT_CATEGORY_TRIG},
  {"factor(p,x)", "factor(", "factor(n) factors the integer n. factor(p,x) factors polynomial p in x. The last argument can be omitted for polynomials in x. The argument list can be extended for multivariate polynomials. For example, factor(p,x,y) factors p over x and then over y.", CAT_CATEGORY_POLYNOMIAL},
  {"factorial(n) / n!", "!", "Returns the product of all positive integers less than or equal to n.", CAT_CATEGORY_OTHER},
  {"filter(f,a,b,...)", "filter(", "Returns f with terms involving a, b, etc. removed.", CAT_CATEGORY_PROG},
  {"float(x)", "float(", "Converts x to a floating point value.", CAT_CATEGORY_OTHER},
  {"floor(x)", "floor(", "Returns the largest integer not greater than x.", CAT_CATEGORY_OTHER},
  {"for(i,j,k,a,b,...)", "for(", "For i equals j through k evaluate a, b, etc.", CAT_CATEGORY_PROG},
  {"gcd(a,b,...)", "gcd(", "Returns the greatest common divisor.", CAT_CATEGORY_CALCULUS},
  {"hermite(x,n)", "hermite(", "Returns the nth Hermite polynomial in x.", CAT_CATEGORY_SPECIAL},
  {"hilbert(n)", "hilbert(", "Returns a Hilbert matrix of order n.", CAT_CATEGORY_LINEARALG},
  {"imag(z)", "imag(", "Returns the imaginary part of complex z.", CAT_CATEGORY_COMPLEXNUM},
  {"integral(f,x)", "integral(", "Returns the integral of f with respect to x.", CAT_CATEGORY_CALCULUS},
  {"inv(m)", "inv(", "Returns the inverse of matrix m.", CAT_CATEGORY_LINEARALG},
  {"isinteger(x)", "isinteger(", "Returns 1 if x is is an integer, zero otherwise.", CAT_CATEGORY_PROG},
  {"isprime(n)", "isprime(", "Returns 1 if n is prime, zero otherwise.", CAT_CATEGORY_CALCULUS},
  {"laguerre(x,n,a)", "laguerre(", "Returns the nth Laguerre polynomial in x. If a is omitted then a = 0 is used.", CAT_CATEGORY_SPECIAL},
  {"lcm(a,b,...)", "lcm(", "Returns the least common multiple.", CAT_CATEGORY_CALCULUS},
  {"leading(p,x)", "leading(", "Returns the leading coefficient of polynomial p in x.", CAT_CATEGORY_POLYNOMIAL},
  {"legendre(x,n,m)", "legendre(", "Returns the nth Legendre polynomial in x. If m is omitted then m = 0 is used.", CAT_CATEGORY_SPECIAL},
  {"log(x)", "log(", "Returns the natural logarithm of x.", CAT_CATEGORY_OTHER},
  {"mag(z)", "mag(", "Returns the magnitude of complex z.", CAT_CATEGORY_COMPLEXNUM},
  {"mod(a,b)", "mod(", "Returns the remainder of a divided by b.", CAT_CATEGORY_CALCULUS},
  {"not(x)", "not(", "Negates the result of predicate expression x.", CAT_CATEGORY_PROG},
  {"nroots(p,x)", "nroots(", "Returns all of the roots, both real and complex, of polynomial p in x. The roots are computed numerically. The coefficients of p can be real or complex.", CAT_CATEGORY_POLYNOMIAL},
  {"number(x)", "number(", "Returns 1 if x is is a number, zero otherwise.", CAT_CATEGORY_PROG},
  {"numerator(x)", "numerator(", "Returns the numerator of expression x.", CAT_CATEGORY_OTHER},
  {"or(a,b,...)", "or(", "Returns the logical \"or\" of predicate expressions.", CAT_CATEGORY_PROG},
  {"outer(a,b,...)", "outer(", "Returns the outer product of tensors.", CAT_CATEGORY_LINEARALG},
  {"polar(z)", "polar(", "Converts complex z to polar form.", CAT_CATEGORY_COMPLEXNUM},
  {"prime(n)", "prime(", "Returns the nth prime number, 1\x10n\x10""10000.", CAT_CATEGORY_CALCULUS},
  {"print(a,b,...)", "print(", "Evaluates expressions and prints the results.. Useful for printing from inside a \"for\" loop.", CAT_CATEGORY_PROG},
  {"product(i,j,k,f)", "product(", "Returns the product of expression f where i is the index, j is the lower bound and k is the upper bound.", CAT_CATEGORY_PROG},
  {"quote(x)", "quote(", "Returns expression x unevaluated.", CAT_CATEGORY_OTHER},
  {"quotient(p,q,x)", "quotient(", "Returns the quotient of polynomials in x.", CAT_CATEGORY_POLYNOMIAL},
  {"rank(a)", "rank(", "Returns the number of indices that tensor a has. A scalar has no indices so its rank is zero.", CAT_CATEGORY_LINEARALG},
  {"rationalize(x)", "rationalize(", "Puts everything over a common denominator.", CAT_CATEGORY_OTHER},
  {"real(z)", "real(", "Returns the real part of complex z.", CAT_CATEGORY_COMPLEXNUM},
  {"rect(z)", "rect(", "Returns complex z in rectangular form.", CAT_CATEGORY_COMPLEXNUM},
  {"roots(p,x)", "roots(", "Returns the values of x such that the polynomial p(x) = 0. The polynomial should be factorable over integers.", CAT_CATEGORY_POLYNOMIAL},
  {"sgn(x)", "sgn(", "Returns -1 if x is negative, 0 if x is zero and 1 if x is positive.", CAT_CATEGORY_OTHER},
  {"simplify(x)", "simplify(", "Returns x in a simpler form.", CAT_CATEGORY_OTHER},
  {"sin(x)", "sin(", "Returns the sine of x.", CAT_CATEGORY_TRIG},
  {"sinh(x)", "sinh(", "Returns the hyperbolic sine of x.", CAT_CATEGORY_TRIG},
  {"sqrt(x)", "sqrt(", "Returns the square root of x.", CAT_CATEGORY_OTHER},
  {"stop()", "stop()", "In a script, it does what it says.", CAT_CATEGORY_PROG},
  {"subst(a,b,c)", "subst(", "Substitutes a for b in c and returns the result.", CAT_CATEGORY_OTHER},
  {"sum(i,j,k,f)", "sum(", "Returns the summation of expression f where i is the index, j is the lower bound and k is the upper bound.", CAT_CATEGORY_PROG},
  {"tan(x)", "tan(", "Returns the tangent of x.", CAT_CATEGORY_TRIG},
  {"tanh(x)", "tanh(", "Returns the hyperbolic tangent of x.", CAT_CATEGORY_TRIG},
  {"taylor(f,x,n,a)", "taylor(", "Returns the Taylor expansion of f of x at a. The argument n is the degree of the expansion. If a is omitted then a = 0 is used.", CAT_CATEGORY_CALCULUS},
  {"test(a,b,c,d,...)", "test(", "If a is true then b is returned else if c is true then d is returned, etc. If the number of arguments is odd then the last argument is returned when all else fails.", CAT_CATEGORY_PROG},
  {"transpose(a,i,j)", "transpose(", "Returns the transpose of tensor a with respect to indices i and j. If i and j are omitted then 1 and 2 are used. Hence a matrix can be transposed with a single argument.", CAT_CATEGORY_LINEARALG},
  {"unit(n)", "unit(", "Returns an n * n identity matrix.", CAT_CATEGORY_LINEARALG},
  {"zero(i,j,...)", "zero(", "Returns a null tensor with dimensions i, j, etc. Useful for creating a tensor and then setting the component values.", CAT_CATEGORY_LINEARALG},
  {">", ">", "\"Greater than\" relational operator. The notation a > b means that a is greater than b.", CAT_CATEGORY_PROG},
  {"<", "<", "\"Less than\" relational operator. The notation a < b means that a is less than b.", CAT_CATEGORY_PROG}
};

int showCatalog(char* insertText) {
  int ret;
  // returns 0 on failure (user exit) and 1 on success (user chose a option)
  MenuItem menuitems[9];
  menuitems[0].text = (char*)"All";
  menuitems[1].text = (char*)"Complex number";
  menuitems[2].text = (char*)"Linear algebra";
  menuitems[3].text = (char*)"Calculus";
  menuitems[4].text = (char*)"Polynomial";
  menuitems[5].text = (char*)"Special";
  menuitems[6].text = (char*)"Program and logic";
  menuitems[7].text = (char*)"Trigonometry";
  menuitems[8].text = (char*)"Other";
  
  Menu menu;
  menu.items=menuitems;
  menu.numitems=9;
  menu.scrollout=1;
  menu.title = (char*)"Function Catalog";
  
  int incat = 1;
  while(incat) {
    int sres = doMenu(&menu);
    ret=0;
    if(sres == MENU_RETURN_SELECTION) {
      if(doCatalogMenu(insertText, menuitems[menu.selection-1].text, menu.selection-1)) {
        ret=1;
        incat = 0;
      }
    } else {
      incat = 0;
    }
  }
  return ret;
}

// 0 on exit, 1 on success
int doCatalogMenu(char* insertText, char* title, int category) {
  MenuItem* menuitems = (MenuItem*)alloca(sizeof(MenuItem)*CAT_COMPLETE_COUNT);
  int cur = 0; int curmi = 0;
  while(cur<CAT_COMPLETE_COUNT) {
    if(completeCat[cur].category == category || category == CAT_CATEGORY_ALL) {
      menuitems[curmi].type = MENUITEM_NORMAL;
      menuitems[curmi].color = TEXT_COLOR_BLACK;
      menuitems[curmi].isfolder = cur; // little hack: store index of the command in the full list in the isfolder property (unused by the menu system in this case)
      menuitems[curmi].text = completeCat[cur].name;
      curmi++;
    }
    cur++;
  }
  
  Menu menu;
  menu.items=menuitems;
  menu.numitems=curmi;
  menu.scrollout=1;
  menu.title = title;
  menu.type = MENUTYPE_FKEYS;
  menu.height = 7;
  while(1) {
    drawFkeyLabels(0x03FC, 0, 0, 0, 0, 0x03FD);
    int sres = doMenu(&menu);
    if(sres == MENU_RETURN_SELECTION || sres == KEY_CTRL_F1) {
      strcpy(insertText, completeCat[menuitems[menu.selection-1].isfolder].insert);
      return 1;
    } else if(sres == KEY_CTRL_F6) {
      textArea text;

      textElement elem[5];
      text.elements = elem;
      text.title = (char*)"Command reference";

      elem[0].text = completeCat[menuitems[menu.selection-1].isfolder].name;
      elem[0].color = COLOR_BLUE;

      elem[1].newLine = 1;
      elem[1].lineSpacing = 8;
      elem[1].text = completeCat[menuitems[menu.selection-1].isfolder].desc;

      text.numelements = 2;
      doTextArea(&text);
    } else if(sres == MENU_RETURN_EXIT) return 0;
  }
  return 0;
}