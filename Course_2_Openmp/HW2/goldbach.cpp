/** 
1.a) Default Scheduling is Static, and the chink size is N / P with P being the number of threads.
1. c) Default dynamic chunk size is 1.
1. e) Default guided chunk size is 1.
1. f) Best time is guided with chunk size 1 : guided(1) (~0.174s) ≈ dynamic(1) (~0.177s) < static(256) (~0.218s). It makes sense because guided is more adaptive to the workload. goldbach(i) gets heavier when i is larger, so adaptive schedules balance the late heavy work; guided can edge dynamic because it starts with big chunks (less overhead) and ends with small chunks (better balance).
*/


#include <iostream>
#include <chrono>
#include <cmath>
#include "omp.h"

#define N 8192

/**
  * Calculer si x est un nombre premier ou pas
  */
bool estPremier(int x)
{
  if (x < 2) { return false; }
  for (int i = 2; i <= x / 2; i++) {
    if (x % i == 0) { return false; }
  }
  return true;
}

/**
  * Calculer le nombre de paires (i, j) tel que i + j = x et i et j sont des nombres premiers.
  */
int goldbach(int x)
{
  int compte = 0;
  if (x <= 2) { return 0; }
  for (int i = 2; i <= x / 2; i++) {
    if (estPremier(i) && estPremier(x - i)) { compte++; }
  }
  return compte;
}

int main()
{
  int goldbachVrai;
  int numPairs[N];
  for (int i = 0; i < N; i++) { numPairs[i] = 0; }

  // La version sequentielle
  auto start = std::chrono::high_resolution_clock::now();
  goldbachVrai = 1;
  for (int i = 4; i < N; i += 2) { 
    numPairs[i] = goldbach(i);
    if (numPairs[i] == 0) { goldbachVrai = 0; }
  }
  if (goldbachVrai) { std::cout << "La conjecture de Goldbach est vrai" << std::endl; }
  else { std::cout << "La conjecture de Goldbach est faux" << std::endl; }
  std::chrono::duration<double> tempsSeq = std::chrono::high_resolution_clock::now() - start;
  std::cout << "Temps sequentiel: " << tempsSeq.count() << "s\n";

  // Parallelisation sans preciser l'ordonnancement.
  start = std::chrono::high_resolution_clock::now();
  #pragma omp parallel for reduction(&:goldbachVrai)
  for (int i = 4; i < N; i+=2){
    numPairs[i] = goldbach(i);
    goldbachVrai &= (numPairs[i] != 0);
  }
  // A FAIRE ...
  std::chrono::duration<double> temps = std::chrono::high_resolution_clock::now() - start;
  std::cout << "Temps ordonnancement par defaut: " << temps.count() << "s\n";

  // Parallelisation avec l'ordonnancement static et taille de bloc 256.
  start = std::chrono::high_resolution_clock::now();
  #pragma omp parallel for schedule(static, 256) reduction(&:goldbachVrai)
  for (int i = 4; i < N; i += 2) {
    numPairs[i] = goldbach(i);
    goldbachVrai &= (numPairs[i] != 0);
  }
  // A FAIRE ...
  temps = std::chrono::high_resolution_clock::now() - start;
  std::cout << "Temps schedule(static, 256): " << temps.count() << "s\n";

  // Parallelisation avec l'ordonnancement dynamic
  start = std::chrono::high_resolution_clock::now();
  #pragma omp parallel for schedule(dynamic) reduction(&:goldbachVrai)
  for (int i = 4; i < N; i += 2) {
    numPairs[i] = goldbach(i);
    goldbachVrai &= (numPairs[i] != 0);
  }
  // A FAIRE ...
  temps = std::chrono::high_resolution_clock::now() - start;
  std::cout << "Temps schedule(dynamic): " << temps.count() << "s\n";


  // Parallelisation avec l'ordonnancement dynamic et taille de bloc 256
  start = std::chrono::high_resolution_clock::now();
  #pragma omp parallel for schedule(dynamic, 256) reduction(&:goldbachVrai)
  for (int i = 4; i < N; i += 2) {
    numPairs[i] = goldbach(i);
    goldbachVrai &= (numPairs[i] != 0);
  }
  // A FAIRE ...
  temps = std::chrono::high_resolution_clock::now() - start;
  std::cout << "Temps schedule(dynamic, 256): " << temps.count() << "s\n";

  // Parallelisation avec l'ordonnancement guided
  start = std::chrono::high_resolution_clock::now();
  #pragma omp parallel for schedule(guided) reduction(&:goldbachVrai)
  for (int i = 4; i < N; i += 2) {
    numPairs[i] = goldbach(i);
    goldbachVrai &= (numPairs[i] != 0);
  }
  // A FAIRE ...
  temps = std::chrono::high_resolution_clock::now() - start;
  std::cout << "Temps schedule(guided): " << temps.count() << "s\n";

  // Parallelisation avec l'ordonnancement guided et taille de bloc 256
  start = std::chrono::high_resolution_clock::now();
  #pragma omp parallel for schedule(guided, 256) reduction(&:goldbachVrai)
  for (int i = 4; i < N; i += 2) {
    numPairs[i] = goldbach(i);
    goldbachVrai &= (numPairs[i] != 0);
  }
  // A FAIRE ...
  temps = std::chrono::high_resolution_clock::now() - start;
  std::cout << "Temps schedule(guided, 256): " << temps.count() << "s\n";

  return 0;
}