                            // Chapter 10 - Programming exercise 1
#include "iostream.h"
#include "person.hpp"
#include "supervsr.hpp"
#include "elemlist.hpp"

employee_list list;

main()
{
supervisor *suppt;
programmer *progpt;
secretary *secpt;
consultant *conpt;

   cout << "XYZ Staff -- note salary is monthly.\n\n";

   suppt = new supervisor;
   suppt->init_data("Big John", 5100, "President");
   list.add_person(suppt);

   progpt = new programmer;
   progpt->init_data("Joe Hacker", 3500, "debugger", "Pascal");
   list.add_person(progpt);

   progpt = new programmer;
   progpt->init_data("OOP Wizard", 7700, "senior analyst", "C++");
   list.add_person(progpt);

   secpt = new secretary;
   secpt->init_data("Tillie Typer", 2200, 1, 85);
   list.add_person(secpt);

   conpt = new consultant;
   conpt->init_data("Tom Consult", 5430, "Planner");
   list.add_person(conpt);

   progpt = new programmer;
   progpt->init_data("Dave Debugger", 5725, "code maintainer", 
                                                "assembly language");
   list.add_person(progpt);

                  // Now display the entire list
   list.display_list();

   cout << "End of employee list.\n";
}




// Result of execution

// XYZ Staff -- note salary is monthly.
//
// Supervisor --> Big John's salary is 5100 and is the President.
//
// Programmer --> Joe Hacker's salary is 3500 and is debugger.
//                Joe Hacker's specialty is Pascal.
//
// Programmer --> OOP Wizard's salary is 7700 and is senior analyst.
//                OOP Wizard's specialty is C++.
//
// Secretary ---> Tillie Typer's salary is 2200.
//                Tillie typer types 85 per minute and can take shorthand.
//
// Consulting --> Tom Consult's salary is 5430 and is the Planner.
//
// Programmer --> Dave Debugger's salary is 5725 and is code maintainer.
//                Dave Debugger's specialty is assembly language.
//
// End of employee list.
