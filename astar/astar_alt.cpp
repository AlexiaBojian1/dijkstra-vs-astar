//ALT / Landmark A*: precomputes distances to a landmark L and sets h(s)=|d(L,s)−d(L,goal)|,
// a triangle-inequality heuristic that tightens estimates without losing admissibility.

#include "../helpers/timer.h"
#include <iostream>