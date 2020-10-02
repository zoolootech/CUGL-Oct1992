                              // Chapter 7 - Programming exercise 2
#ifndef CARHPP
#define CARHPP

#include "vehicle.hpp"

class car : public vehicle {
   int passenger_load;
public:
   void initialize(int in_wheels, float in_weight, int people = 4);
   int passengers(void);
};

#endif




// Result of execution
//
// (this file cannot be executed)
