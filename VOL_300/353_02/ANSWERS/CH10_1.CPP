                              // Chapter 9 - Programming exercise 1
#include <iostream.h>

class vehicle {
   int wheels;
   float weight;
public:
   void message(void) { cout << "Vehicle message\n";}
};


class car : public vehicle {
   int passenger_load;
public:
   void message(void) { cout << "Car message\n";}
};


class truck : public vehicle {
   int passenger_load;
   float payload;
public:
   int passengers(void) {return passenger_load;}
};


class boat : public vehicle {
   int passenger_load;
public:
   int passengers(void) {return passenger_load;}
   void message(void) { cout << "Boat message\n";}
};


main()
{
vehicle *unicycle;
car *sedan;
truck *semi;
boat *sailboat;

   unicycle = new vehicle;
   unicycle->message();
   sedan = new car;
   sedan->message();
   semi = new truck;
   semi->message();
   sailboat = new boat;
   sailboat->message();

   delete unicycle;
   delete sedan;
   delete semi;
   delete sailboat;
}



// Result of execution
//
// Vehicle message
// Car message
// Vehicle message
// Boat message
