                              // Chapter 2 - Programming exercise 1
#include <iostream.h>

enum game_result {win, lose, tie, cancel, forfeit};

main()
{
game_result result;
enum game_result omit = cancel;

   for (result = win;result <= forfeit;result++) {
      if (result == forfeit)
         cout << "We had to forfeit the game\n";
      else if (result == omit) 
         cout << "The game was cancelled\n";
      else {
         cout << "The game was played ";
         if (result == win)
            cout << "and we won!";
         if (result == lose)
            cout << "and we lost.";
         cout << "\n";
      }
   }
}




// Result of execution
//
// The game was played and we won!
// The game was played and we lost.
// The game was played
// The game was cancelled
// We had to forfeit the game
