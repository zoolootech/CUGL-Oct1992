/*
 * Example program
 */

#include <stream.hpp>
#include "flash.hpp"

miracl precision=(-35);

main()
{ /* Brents example program */
    Flash x;
    cout << pi() << "\n";
    x=exp(pi()*sqrt((Flash)"163/9"));
    cout << x << "\n";
    cout << pow(x,3) << "\n";
}

