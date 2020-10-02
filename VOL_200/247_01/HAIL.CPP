/*
 *   Program to investigate hailstone numbers.
 *   Gruenberger F. 'Computer Recreations' Scientific American. April 1984.
 */

#include <stream.hpp>
#include "big.hpp"

miracl precision=100;

main()
{  /*  hailstone numbers  */
    int iter;
    Big x,y,r,mx;
    iter=0;
    cout << "number = ";
    cin >> x;
    do
    {
        if (x>mx) mx=x;
        r=x%2; 
        if (r!=0) x=3*x+1;
        else      x/=2;
        cout << x << "\n";
        iter++;
    } while (x!=1);
    cout <<  "path length = " << iter << "\n";
    cout <<  "maximum = " << mx << "\n";
} 

