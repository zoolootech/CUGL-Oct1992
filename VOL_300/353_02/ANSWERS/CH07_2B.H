                              // Chapter 7 - Programming exercise 2
#ifndef TRUCKHPP
#define TRUCKHPP

#include "vehicle.hpp"

class truck : public vehicle {
   int passenger_load;
   float payload;
public:
   void init_truck(int how_many = 2, float max_load = 24000.0);
   float efficiency(void);
   int passengers(void);
};

#endif




// Result of execution
//
// (this file cannot be executed)
