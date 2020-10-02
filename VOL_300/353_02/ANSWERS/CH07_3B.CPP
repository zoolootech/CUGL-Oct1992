                             // Chapter 7 - Programming exercise 3
#include <truck.h>

void truck::init_truck(int how_many = 2, float max_load = 24000.0)
{
   passenger_load = how_many;
   payload = max_load;
}


float truck::efficiency(void)
{
   return payload / (payload + get_weight());
}


int truck::passengers(void)
{
   return passenger_load;
}


int truck::total_weight(void)
{
   return (payload + get_weight());
}



// Result of execution
//
// (this file cannot be executed)
