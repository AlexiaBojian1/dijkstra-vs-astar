#!/usr/bin/env bash
set -e

GRAPH=data/graph_huge.txt   # choose data/graph_huge.txt for monster test
RUNS=2
OUT=benchmarks.csv

printf "Algorithm,Time_ms,Nodes,Mem_KB\n" > "$OUT"

ALGS=(
  dijkstra/dijk_lazy
  dijkstra/dijk_decKey
  dijkstra/dijk_Fib
  astar/astar_weighted
  astar/astar_alt
)

for ALG in "${ALGS[@]}"; do
  echo "▶  $ALG"
  sum_t=0; sum_n=0; sum_m=0
  for i in $(seq 1 $RUNS); do
    LINE=$("./$ALG" < "$GRAPH" | grep 'Time(ms)')
    # Parse the single metrics line
    t=$(echo "$LINE" | sed -n 's/.*Time(ms): *\([0-9]\+\).*/\1/p')
    n=$(echo "$LINE" | sed -n 's/.*Nodes: *\([0-9]\+\).*/\1/p')
    m=$(echo "$LINE" | sed -n 's/.*MaxRSS(KB): *\([0-9]\+\).*/\1/p')

    t=${t//[[:space:]]/}
    n=${n//[[:space:]]/}
    m=${m//[[:space:]]/}

    sum_t=$((sum_t + t))

  done
  avg_t=$((sum_t / RUNS)); avg_n=$((sum_n / RUNS)); avg_m=$((sum_m / RUNS))
  printf "%s,%d,%d,%d\n"  "$(basename "$ALG")"  "$avg_t" "$avg_n" "$avg_m" >> "$OUT"
done

echo "✅  Results → $OUT"
