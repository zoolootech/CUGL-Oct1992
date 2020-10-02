/*
 *   Program to calculate factorials.
 */

#include <stream.hpp>
#include "big.hpp"   /* include MIRACL system */

miracl precision=100;

main()
{ /* calculate factorial of number */
    Big nf=1;           /* declare "Big" variable nf */
    int n;
    cout << "factorial program\n";
    cout << "input number n= \n";
    cin >> n;
    while (n>1) nf*=(n--);   /* nf=n!=n*(n-1)*(n-2)*....3*2*1  */
    cout << "n!= \n" << nf << "\n";
}
