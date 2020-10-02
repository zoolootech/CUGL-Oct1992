                             // Chapter 7 - Programming exercise 3
#include "vehicle.hpp"

class truck : public vehicle {
   int passenger_load;
   float payload;
public:
   void init_truck(int how_many = 2, float max_load = 24000.0);
   float efficiency(void);
   int passengers(void);
   int total_weight(void);
};




// Result of execution
//
// (this file cannot be executed)
