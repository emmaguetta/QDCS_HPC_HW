/** It goes faster using more threads.
*/

#include <cstdio>
#include <cstdlib>
#include "omp.h"

void printUsage(int argc, char **argv)
{
  printf("Usage: %s N\n", argv[0]);
  printf("Example: %s 13\n", argv[0]);
}

int fib(int n)
{
    if (n<=1) { return n;};
    int x, y;
    #pragma omp task shared(x)
    x = fib(n-1);
    #pragma omp task shared(y)
    y = fib(n-2);
    #pragma omp taskwait
    return x + y;
    }


int main(int argc, char **argv)
{
  // Check the validity of command line arguments and print usage if invalid
  // Verifier la validite des arguments fournis a la ligne de commande et afficher l'utilisation si invalid
  if (argc < 2) { 
    printUsage(argc, argv);
    return 0;
  }

  // Read the index of the Fibonacci number to compute
  // Lire l'indice du nombre Fibonacci a calculer
  const int N = atoi(argv[1]);

  // Mesure de temps et exécution parallèle avec tâches
  double t0 = omp_get_wtime();
  int threads = 0;
  #pragma omp parallel
  {
    // Un seul thread lance le calcul, mais la récursion crée des tâches
    #pragma omp single
    {
        threads = omp_get_num_threads();
        int result = fib(N);
        printf("Fibonacci de %d est %d\n", N, result);
    }
  }
  double t1 = omp_get_wtime();
  printf("Threads=%d, Time=%.6fs\n", threads, t1 - t0);

  return 0;
}




