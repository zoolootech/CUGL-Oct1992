A Poor Man's Solution to the Traveling Salesman Problem

Kevin E Knauss                                                       12 Mar 89

Given a map and a means of transportation, a competent traveling salesman can
pick a reasonable route to all his customers.  The route he picks needn't be 
optimal just practical.  In this article, we'll explore a hypothetical 
salesman's intuitive approach to finding a practical route to all his 
customers and its implementation in the C programming language.  In an attempt
to find elegant optimal solutions to tough problems, we often overlook
solutions which appear to be rather "brutish."  Upon closer inspection,
however, we see that these solutions are more elegant than they appear and,
better yet, they work!

BACKGROUND

Routing and scheduling problems are inherently difficult to solve because they
often require total enumeration of all possible outcomes.  As the number of
data points in the problem increases, the possible outcomes increase
exponentially.  With the Traveling Salesman Problem (TSP) for instance,
studies have shown that an algorithm which yields an exact solution is
relatively infeasible for networks containing in excess of 100 points.  In
fact, there are problems with as few as 48 cities for which the best answer
found has not been proven to be optimal.  By using heuristics of various
types, one is enabled to find feasible (though not necessarily optimal)
solutions to these otherwise "unsolvable" problems.

The TSP simply stated is: "A salesman, starting in one city, wishes to visit 
each nÄ1 other cities once and only once and return to the start.  In what
order should he visit the cities to minimize the total distance traveled?" (8) 
This may seem too trivial a task to have generated active research for over
three decades (the literature I've researched goes back to 1958), but there 
are many practical applications for the solution of this basic problem.
Automated drilling machines and the newer laser drills used to drill printed
circuit boards, for example, may have hundreds or thousands of holes to drill
and often spend as much time traveling to a position for a hole as they do
drilling it.  Programming efficient head travel for these machines could make
the difference for a company to turn a profit or loss.  Solve the more basic
TSP, and the marginal printed circuit board company may be able to stay in
business by applying the same techniques.

The TSP is one which is, to quote an old cliche, "easier said than done."
That is, it is easy to explain the problem but difficult to solve; at least it
seems difficult to solve when we look at many of the purely mathematical
models that are, too often, not related back to the original problem.  I chose
to attack the TSP in a simplistic manner in an attempt to find one or more
algorithms which would approximate a person's intuitive approach.  In so 
doing, I was able to find an efficient way to "solve" the problem by producing
acceptable results to largeÄscale traveling salesman problems.  (Note that the
word "acceptable" implies that judgments are required.)

TERMINOLOGY

Before we begin traveling around, a review of TSP terminology is in order.
We'll begin with the city, our most basic term.  This is what will be visited 
and may also be referred to as a town, point, node, or vertex.  One goes from
one city to the next by traveling a given distance or incurring a specified
cost.  The terms link, arc, and edge are also used in place of distance or
cost.  The collection of all the cities and the distances between each pair is
a network or graph and is often represented by a distance matrix.  A salesman
will follow a route to visit each of the cities in the network, and this route
may also be called a tour, path, or circuit.  Finally, if we remove two or
more links from the completed tour, we will break it into subÄpaths or chains
of cities.

The distance matrix is a two dimensional array where the horizontal or row
vector (dimension) is identical to the vertical or column vector.  The cell
found at each intersection contains the distance or cost between the city
represented by the horizontal coordinate and the city represented by the
vertical coordinate.  Those familiar with graph theory haven't seen anything 
new here.  If you've seen a lot of these terms for the first time, however, 
don't be afraid to refer back, for I'll be using many of them interchangeably.

APPROACH

Let's now consider the problem in terms of a salesman who must visit a dozen 
or so cities in the state of Hypothetica.  Since the salesman must leave and
return to the same city and visit all other cities in the process, his tour
will be some sort of loop through the state.  Obviously, as a loop is
unbroken, one may start at any point on the the tour and still trace the same
loop.  Thus the starting city is of no consequence; rather we want to find the
best route irregardless of the salesman's starting point.

Intuitively, one would want to travel to cities nearby and to cities near
those.  We can build a procedure based on this thought by first finding the
closest two cities and then continuing to the next closest city that hasn't 
been visited.  This should produce a fairly good tour, or at least would seem
so at first.  It may turn out that this tour isn't optimal, but it's a
reasonable solution for starters.

As the cities are exhausted from our network, we have fewer choices to make.
Intuitively, we may reason that the choices left to us may not be as good as
those we're offered in the early stages of tour building.  Our salesman may be 
forced to backtrack and cross previously traversed arcs.  If we check the
proximity of neighboring cities, however, especially those near the end of the
initial tour, we may be able to find improvements.

One approach we may try involves the removal of a single city from the tour
and testing it between each pair of cities in the remaining tour.  Once we've 
tested it in each location, we'll place it in the location where the overall 
circuit cost is lowest (i.e. the shortest distance the salesman must travel).
This same approach may be tried with chains of cities of varying lengths, but
with chains we must also check for orientation (that is try the chain both
frontward and backward between each pair of cities).  This leaves us with our
last thought of simply checking the orientation of a chain in its original
location.  If you think a picture is worth a thousand words, see Figure 1 so
we can cut 4,000 words from this article.  By testing the proximity of every
city or the proximity and orientation of every chain, we can be fairly
confident that any ill effects produced by our original technique will be
cleaned up.

If we look through related literature, we find that our tour building and
improvement techniques have already been studied and named.  Our tour building
algorithm is known as the nearest neighbor or greedy algorithm.  Our tour
improvement algorithms generally fall into a category known as kÄoptimality or
kÄopt for short.  A tour is said to be kÄoptimal if we are unable to improve
it by removing any k arcs and replacing them with k others.  Checking chain
orientation in place is the same as removing two arcs and replacing them with
two others and is thus the 2Äopt algorithm.  Likewise, chain proximity and
orientation is the 3Äopt algorithm with point proximity a special case where
the chain to be tested has length one.  Even though these improvement
techniques are related, we'll evaluate each on its own merits.

IMPLEMENTATION

To evaluate the intuitive approach we may embark upon an elaborate
mathematical analysis that may or may not produce any conclusive results, or
we may implement the solutions in practical models that may be run against
live data.  If this was a scientific journal, we'd follow the mathematical 
tack; but since this is a practical journal, we'll try the modeling approach.

The main functions are programmed in their own modules called:  NearNeighbor,
PointOpt, TwoOpt, Hybrid, and ThreeOpt (listings 1 through 5 respectively).
NearNeighbor generates the initial tour from the distance matrix while the
other routines take turns improving it.  PointOpt performs point proximity
improvement only, and TwoOpt performs only chain orientation improvement.
Hybrid combines point proximity and chain orientation improvements while
ThreeOpt adds chain proximity and orientation.  The nearest neighbor, 2ÄOpt,
and 3ÄOpt algorithms have been studied in detail within the field, but are
normally regarded as independent techniques.  To my knowledge, this is the
first that point proximity has been considered either independently (PointOpt)
or in conjunction with the 2ÄOpt algorithm (Hybrid).

We'll use six distance matrices found in the literature to test our procedures 
since these networks have known optima (or at least a best known solution as
is the case of the 48 city problem).  We'll need to know the tour length each 
procedure produces and the time it takes to find the tour.  We can calculate
from this information how much improvement is made by each technique and what
percentage each solution is from the known optimum.  To see how the
improvement techniques work on different initial tours, we'll reverse the 
initial nearest neighbor tour and generate a bad initial tour.

To capture the time, we'll need a system dependent routine.  GetTime samples 
the clock counter by issuing an interrupt under MSÄDOS; ElapsedTime calls
GetTime and compares the new time with a previous time passed in.  Listing 6
shows GetTime implemented using the MIX C compiler for MSÄDOS and ElapsedTime
in a plain vanilla implementation.  For the bad initial tour, we'll simply 
reverse the logic of the nearest neighbor algorithm to generate a farthest
neighbor tour (listing 7).  The driver program (listing 8) is far from
elegant but gets the job done.

OBSERVATIONS

One might assume that, since it embodies all the techniques used in the other
improvement algorithms, the 3ÄOpt algorithm would produce a tour at least as
good as the others.  Our results show that one might be wrong in such an
assumption!  In fact, the 3ÄOpt algorithm only found the best solution in the
two smallest problems and other algorithms found the same solutions (see
Figure 2).  The total time to run the three tour building routines and the
three lesser improvement routines on each is less than the time needed to run
the 3ÄOpt routine just once on one of the larger problems (see Figure 3). This
indicates that the 3ÄOpt algorithm may not be very valuable as a tour
improvement algorithm.

The independent point proximity algorithm is likewise not very valuable.  It
was 15% faster than the Hybrid routine overall but fell well behind in
performance.  PointOpt found the best solution in the 10 city problem, but
Hybrid found the same solution.  Hybrid outperformed PointOpt in all the other
solutions so nothing is gained by running both routines.

We can see that the TwoOpt and Hybrid routines compliment each other very
nicely.  Where Hybrid didn't find the best solution, TwoOpt did.  The 2ÄOpt 
algorithm was consistently the fastest and the point proximity 2ÄOpt hybrid
found the best answer in four of the six problems.  Their combined times plus
the time to build the initial tour was comparable to the time required to read
the distance matrix on my 12Mhz AT clone with 28ms access hard drive.

One thing that may be a surprise, is that our tour improvement algorithms are
dependent upon how compatible the initial tour is with the techniques being
applied and not necessarily how close to optimum that tour is.  In all but one
problem, the best improvement was found when starting from the nearest
neighbor solution.  In the 42 city problem, however, the best improvement was
found by starting with the farthest neighbor solution.  In addition to being
found from the nearest neighbor solution, the same improvement was found in
less time from the farthest neighbor solution in the 10 and 20 city problems.

The MIX C compiler/linker with its .COM executable files causes some problems
for large scale applications such as the drilling machines.  I had no problem
with our small to medium scale problems, but when I compiled the program with
dimensions over 100 the program ran out of space.  Dynamic storage allocation
won't cure the problem since the heap space for dynamic storage allocation and 
the stack space for static data structures all come from the same pot. Answers
will have to come from a C environment that allows for .EXE executable files
or the use of disk resident dynamic arrays.  The latter of these will degrade
execution time, but taking a long time to find a solution is better than
taking a short time to not find one at all!  Note that by using the function
ArcCost to access distance matrix data we've made it easy to try differing 
storage methods.

CONCLUSION

Following an intuitive approach to a problem and implementing that approach
can often produce very acceptable results.  Though there can be no substitute
for thorough analysis, neither can there be a substitute for experimentation
and testing of hypotheses.  The modularity of C with its procedures and
functions allows the building blocks of experimentation to become the building
blocks of application.  With our TSP modeling, we need only develop a new
driver program to build an efficient production package.
