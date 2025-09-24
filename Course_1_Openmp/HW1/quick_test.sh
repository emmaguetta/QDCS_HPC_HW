#!/bin/bash

echo "=== Test rapide Mergesort ==="

# Compiler
g++-openmp mergesort_sequential.cpp -o mergesort_sequential
g++-openmp mergesort.cpp -o mergesort

# Test avec 10000 éléments
echo "Test avec 10000 éléments :"
echo ""

echo "Version séquentielle :"
./mergesort_sequential 10000

echo ""
echo "Version parallèle :"
./mergesort 10000

# Nettoyage
rm mergesort_sequential mergesort