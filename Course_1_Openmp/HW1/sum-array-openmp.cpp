#include <chrono>
#include <iostream>
#include <vector>
#include <cstdio>
#include "omp.h"

int main()
{
  int i;
  int N = 10000000;
  std::vector<double> A(N);
  double sum = 0.0;

  auto start = std::chrono::high_resolution_clock::now();

  // Partie a) - Initialisation du tableau A[i] = i (VERSION SÉQUENTIELLE)
  printf("=== VERSION SÉQUENTIELLE ===\n");
  printf("Initialisation du tableau de %d éléments...\n", N);
  for (int i = 0; i < N; i++) {
    A[i] = i;
  }
  printf("Initialisation terminée.\n");
  
  // Partie c) - Initialisation du tableau A[i] = i (VERSION PARALLÈLE)
  printf("\n=== VERSION PARALLÈLE ===\n");
  printf("Initialisation parallèle du tableau de %d éléments...\n", N);
  
  #pragma omp parallel
  {
    #pragma omp for
    for (int i = 0; i < N; i++) {
      A[i] = i;
    }
  }
  printf("Initialisation parallèle terminée.\n");
  
  // Partie b) - Calcul de la somme de tous les éléments (VERSION SÉQUENTIELLE)
  printf("\n=== VERSION SÉQUENTIELLE ===\n");
  printf("Calcul de la somme...\n");
  for (int i = 0; i < N; i++) {
    sum += A[i];
  }
  printf("Calcul de la somme terminé.\n");
  
  // Partie d) - Calcul de la somme avec omp sections (4 sections)
  printf("\n=== VERSION PARALLÈLE AVEC SECTIONS ===\n");
  printf("Calcul de la somme avec 4 sections...\n");
  
  double sum_parallel = 0.0;  // Nouvelle variable pour la somme parallèle
  
  #pragma omp parallel sections num_threads(4)
  {
    // Section 1: éléments 0 à N/4-1
    #pragma omp section
    {
      double local_sum = 0.0;
      for (int i = 0; i < N/4; i++) {
        local_sum += A[i];
      }
      #pragma omp critical
      {
        sum_parallel += local_sum;
        printf("Section 1 (0 à %d): somme locale = %.0f\n", N/4-1, local_sum);
      }
    }
    
    // Section 2: éléments N/4 à N/2-1
    #pragma omp section
    {
      double local_sum = 0.0;
      for (int i = N/4; i < N/2; i++) {
        local_sum += A[i];
      }
      #pragma omp critical
      {
        sum_parallel += local_sum;
        printf("Section 2 (%d à %d): somme locale = %.0f\n", N/4, N/2-1, local_sum);
      }
    }
    
    // Section 3: éléments N/2 à 3N/4-1
    #pragma omp section
    {
      double local_sum = 0.0;
      for (int i = N/2; i < 3*N/4; i++) {
        local_sum += A[i];
      }
      #pragma omp critical
      {
        sum_parallel += local_sum;
        printf("Section 3 (%d à %d): somme locale = %.0f\n", N/2, 3*N/4-1, local_sum);
      }
    }
    
    // Section 4: éléments 3N/4 à N-1
    #pragma omp section
    {
      double local_sum = 0.0;
      for (int i = 3*N/4; i < N; i++) {
        local_sum += A[i];
      }
      #pragma omp critical
      {
        sum_parallel += local_sum;
        printf("Section 4 (%d à %d): somme locale = %.0f\n", 3*N/4, N-1, local_sum);
      }
    }
  }
  
  printf("Calcul de la somme parallèle terminé.\n");
  printf("Somme séquentielle: %.0f\n", sum);
  printf("Somme parallèle: %.0f\n", sum_parallel);

  std::cout << "Sum is " << sum << std::endl;
  std::chrono::duration<double> time = std::chrono::high_resolution_clock::now() - start;
  std::cout << "Execution time: " << time.count() << "s\n";

  return 0;
}