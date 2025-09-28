#!/bin/zsh
set -e
cd "$(dirname "$0")"
LIBOMP_PREFIX=$(brew --prefix libomp 2>/dev/null || echo "/opt/homebrew/opt/libomp")
SIZES=(200000)
THRESHOLDS=(512 1024 2048 4096 8192 12000 16000 24000 32000 48000)
echo "thresh,size,time_s"
for size in ${SIZES[@]}; do
  for th in ${THRESHOLDS[@]}; do
    clang++ -O3 -std=c++17 -Xpreprocessor -fopenmp -I"$LIBOMP_PREFIX/include" -L"$LIBOMP_PREFIX/lib" -lomp -DTHRESH=$th mergesort.cpp -o mergesort >/dev/null
    out=$(./mergesort $size 2>/dev/null | sed -n "s/Sorting took \([0-9.]*\) seconds\./\1/p")
    echo "$th,$size,$out"
  done
done
