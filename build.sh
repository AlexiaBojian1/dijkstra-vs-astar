#!/usr/bin/env bash
set -e          # stop on first error

echo "⏳  Building Dijkstra variants"
g++ -O3 -std=c++17 dijkstra/dijk_lazy.cpp     -o dijkstra/dijk_lazy
g++ -O3 -std=c++17 dijkstra/dijk_decKey.cpp   -o dijkstra/dijk_decKey
g++ -O3 -std=c++17 dijkstra/dijk_Fib.cpp      -o dijkstra/dijk_Fib

echo "⏳  Building A* variants"
g++ -O3 -std=c++17 astar/astar_alt.cpp        -o astar/astar_alt
g++ -O3 -std=c++17 astar/astar_weighted.cpp   -o astar/astar_weighted
g++ -O3 -std=c++17 astar/astar_alt.cpp  -DHZERO -o astar/astar_zero
# ^ the last line re-compiles the same file with a compile-time flag that forces h=0.
#   If you have a separate astar_zero.cpp, just replace this line with its filename.

echo "✅  Build finished"
