/**
 * It doesn't go faster using more threads. :
Fibonacci de 40 est 102334155
Threads=1, Time=0.000077s
Fibonacci de 40 est 102334155
Threads=2, Time=0.000103s
Fibonacci de 40 est 102334155
Threads=4, Time=0.000182s
Fibonacci de 40 est 102334155
Threads=8, Time=0.010904s

Because the computation is sequential, it doesn't make sense to use more threads.
 */


#include <cstdio>
#include <cstdlib>
#include "omp.h"
#include <vector>

void printUsage(int argc, char **argv)
{
  printf("Usage: %s N\n", argv[0]);
  printf("Example: %s 13\n", argv[0]);
}

int fib_iter(int n) {
  if (n <= 1) return n;

  std::vector<int> F(n + 1);
  F[0] = 0;
  F[1] = 1;

  for (int i = 2; i <= n; i++) {
      #pragma omp task depend(in:F[i-1],F[i-2]) depend(out:F[i]) shared(F)
      {
          F[i] = F[i-1] + F[i-2];
      }
  }
  #pragma omp taskwait

  int result = F[n];
  return result;
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

  // Allocate and initialize the array containing Fibonacci numbers
  // Allouer et initialiser le tableau contenant les nombres de Fibonacci
  int fib[1000];
  fib[0] = 0;
  fib[1] = 1;

  // Create threads before creating tasks. Only one thread will create tasks, other threads will execute them when they
  // are idle. fib is an array pointer and N is a constant so we can share them among threads.
  // Creer les threads avant de creer les taches. Seulement un thread s'occupera de la creation des taches, les autres
  // threads executerons les taches tant qu'ils sont disponibles.
  // TODO / A FAIRE ...

// Exécuter le calcul en parallèle (création des tâches dans fib_iter)

double t0 = omp_get_wtime();
int threads = 0;
#pragma omp parallel
{
  #pragma omp single
  {
      threads = omp_get_num_threads();
      int result = fib_iter(N);
      printf("Fibonacci de %d est %d\n", N, result);
  }
}
double t1 = omp_get_wtime();
printf("Threads=%d, Time=%.6fs\n", threads, t1 - t0);

  return 0;
}

