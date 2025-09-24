/*
 * b) It does not work.  
 * The problem is that multiple threads are trying to write to the same variable pi at the same time, causing race conditions. 
 * When thread 1 reads pi=0.1, calculates +0.2, and tries to write pi=0.3, thread 2 might also read pi=0.1 simultaneously, calculate +0.3, and overwrite thread 1's result with pi=0.4, leading to data loss and incorrect final results.
*/




#include <chrono>
#include <iostream>
#include "omp.h"

inline double f(double x)
{
  return (4 / (1 + x * x));
}

int main()
{
  int i;
  const int N = 100000000;
  double pi = 0.0;

  // Compute pi sequentially
  // Calculer le pi en sequentiel
  auto start = std::chrono::high_resolution_clock::now();
  
  double s = 1.0 / N;
  for (i = 0; i < N; i++) {
    double x1 = i * s;
    double x2 = (i + 1) * s;
    pi += s * (f(x1) + f(x2)) / 2.0;
  }
  
  std::cout << "pi = " << pi << std::endl;
  std::chrono::duration<double> tempsSeq = std::chrono::high_resolution_clock::now() - start;
  std::cout << "Temps sequentiel: " << tempsSeq.count() << "s\n";

// Calculer pi with omp for and no reduction
  pi = 0.0;
  start = std::chrono::high_resolution_clock::now();
  #pragma omp parallel default(none) shared(pi, N, s) num_threads(8)
  {
    #pragma omp for
    for (i = 0; i < N; i++) {
      double x1 = i * s;
      double x2 = (i + 1) * s;
      pi += s * (f(x1) + f(x2)) / 2.0;
    }
  }
  std::cout << "pi = " << pi << std::endl;
  std::chrono::duration<double> tempsOmpForNoRed = std::chrono::high_resolution_clock::now() - start;
  std::cout << "Temps parallel omp for sans reduction: " << tempsOmpForNoRed.count() << "s\n";

  // Compute pi with omp for and reduction
  // Calculer le pi avec omp for et reduction
  pi = 0.0;
  start = std::chrono::high_resolution_clock::now();
  
  #pragma omp parallel for reduction(+:pi) num_threads(8)
  for (i = 0; i < N; i++) {
    double x1 = i * s;
    double x2 = (i + 1) * s;
    pi += s * (f(x1) + f(x2)) / 2.0;
  }
  
  std::cout << "pi = " << pi << std::endl;
  std::chrono::duration<double> tempsOmpFor = std::chrono::high_resolution_clock::now() - start;
  std::cout << "Temps parallel omp for: " << tempsOmpFor.count() << "s\n";

  // Compute pi with a loop parallalized "by hand"
  // Calculer le pi avec boucle parallele faite a la main
  pi = 0.0;
  start = std::chrono::high_resolution_clock::now();
  
  const int P = 8; // Number of threads
  const int chunk_size = N / P; // Size of each thread's domain
  
  #pragma omp parallel default(none) shared(pi, N, s, P, chunk_size) num_threads(P)
  {
    int thread_id = omp_get_thread_num();
    int begin = thread_id * chunk_size;
    int end = (thread_id == P - 1) ? N : (thread_id + 1) * chunk_size; // Last thread takes remaining elements
    
    double partial_pi = 0.0;
    
    // Each thread computes its partial sum
    for (int i = begin; i < end; i++) {
      double x1 = i * s;
      double x2 = (i + 1) * s;
      partial_pi += s * (f(x1) + f(x2)) / 2.0;
    }
    
    // Merge partial values into final pi using atomic operation
    #pragma omp atomic
    pi += partial_pi;
  }
  
  std::cout << "pi = " << pi << std::endl;
  std::chrono::duration<double> tempsForMain = std::chrono::high_resolution_clock::now() - start;
  std::cout << "Temps parallel for a la main: " << tempsForMain.count() << "s\n";

  return 0;
}