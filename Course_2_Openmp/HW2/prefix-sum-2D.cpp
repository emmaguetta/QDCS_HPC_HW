/** 
 * a) We cannot use omp for to parallelize the computation of B[i][j],
 * because each element depends on previously computed values 
 * (B[i-1][j], B[i][j-1], and B[i-1][j-1]).
 * The calculation has inherent data dependencies, so the updates 
 * must follow a specific sequential order.

 b) Time (task) > Time (seq). Because it introduces a lot of overhead.


*/


#include <iostream>
#include <chrono>
#include "omp.h"

#define N 1024
#define K 64
#define NTASKS (N / K)
#define NB (N / K)

double A[N][N];
double B[N][N];
bool deps[NTASKS + 1][NTASKS + 1];

void printArray(double tab[N][N])
{
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      printf("%4.0lf ", tab[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

int main(int argc, char **argv)
{
  // Initialize the array A[i][j] = i + j in parallel.
  // Initaliser le tableau A[i][j] = i + j en parallele.

  // Version 1: Using omp for loop. We can collapse two loops since each iteration (i, j) is independent
  // Version 1: Avec boucle omp for. On peut faire collapse pour deux boucles car chaque iteration (i, j) est
  // independante
  // TODO / A FAIRE ...

// Sequentiel + mesure
for (int i = 0; i < N; i++){
    for (int j = 0; j < N; j++ ){
        A[i][j] = i + j;
    }
}
  auto t0 = std::chrono::high_resolution_clock::now();
B[0][0] = A[0][0];

for (int i = 1; i < N; i++){
    B[i][0] = B[i-1][0] + A[i][0];
}

for (int j = 1; j < N; j++){
    B[0][j] = B[0][j-1] + A[0][j];
}


for (int i = 1; i < N; i++){
    for (int j = 1; j < N; j++)
    B[i][j] = B[i][j-1] + B[i-1][j] - B[i-1][j-1] + A[i][j];
}
  auto t1 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> seq_time = t1 - t0;

if (N < 20) {
    printf("Array A:\n");
    printArray(A);
    printf("Array B:\n");
    printArray(B);
  }
  std::cout << "Temps seq: " << seq_time.count() << " s\n";

  // Parallèle
# pragma omp parallel default(none) shared(A)
{
    # pragma omp for collapse(2) 
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++ ){
            A[i][j] = i + j;
    }
}

}

auto p0 = std::chrono::high_resolution_clock::now();
#pragma omp parallel
{
  #pragma omp single
  {
    // Case de base
    B[0][0] = A[0][0];

    // Première colonne
    for (int i = 1; i < N; i++) {
      #pragma omp task depend(in: B[i-1][0]) depend(out: B[i][0])
      {
        B[i][0] = B[i-1][0] + A[i][0];
      }
    }

    // Première ligne
    for (int j = 1; j < N; j++) {
      #pragma omp task depend(in: B[0][j-1]) depend(out: B[0][j])
      {
        B[0][j] = B[0][j-1] + A[0][j];
      }
    }

    // Cas général
    for (int i = 1; i < N; i++) {
      for (int j = 1; j < N; j++) {
        #pragma omp task depend(in: B[i][j-1]) depend(in: B[i-1][j]) depend(in: B[i-1][j-1]) depend(out: B[i][j])
        {
          B[i][j] = B[i][j-1] + B[i-1][j] - B[i-1][j-1] + A[i][j];
        }
      }
    }
  }
}
auto p1 = std::chrono::high_resolution_clock::now();
std::chrono::duration<double> par_time = p1 - p0;


if (N < 20) {
    printf("Array A:\n");
    printArray(A);
    printf("Array B:\n");
    printArray(B);
  }
  std::cout << "Temps par (tasks): " << par_time.count() << " s\n";




  // Version 2: Using tasks, each task initializes a block of size K x K. There are (N / K) x (N / K) tasks in total
  // Version 2: Avec taches, chaque tache initialise un bloc de taille K x K. Il y a (N / K) x (N / K) taches au total
  // TODO / A FAIRE ...


  if (N < 20) {
    printf("Array A:\n");
    printArray(A);
  }

  // TODO / A FAIRE ...
  // Non-blocked version for computing B using N x N tasks
  // Version sans bloc pour calculer B avec N x N taches


  // TODO / A FAIRE ... (version par blocs avec dependances) + mesure
  auto tb0 = std::chrono::high_resolution_clock::now();
  #pragma omp parallel
  {
    #pragma omp single
    {
      for (int bi = 0; bi < NB; bi++) {
        for (int bj = 0; bj < NB; bj++) {

          if (bi == 0 && bj == 0) {
            #pragma omp task depend(out: deps[bi][bj])
            {
              for (int i = bi*K; i < (bi+1)*K; i++) {
                for (int j = bj*K; j < (bj+1)*K; j++) {
                  if (i == 0 && j == 0) {
                    B[0][0] = A[0][0];
                  } else if (i == 0) {
                    B[0][j] = B[0][j-1] + A[0][j];
                  } else if (j == 0) {
                    B[i][0] = B[i-1][0] + A[i][0];
                  } else {
                    B[i][j] = B[i][j-1] + B[i-1][j] - B[i-1][j-1] + A[i][j];
                  }
                }
              }
            }
          } else {
            if (bi > 0 && bj > 0) {
              #pragma omp task depend(in: deps[bi-1][bj], deps[bi][bj-1], deps[bi-1][bj-1]) depend(out: deps[bi][bj])
              {
                for (int i = bi*K; i < (bi+1)*K; i++) {
                  for (int j = bj*K; j < (bj+1)*K; j++) {
                    B[i][j] = B[i][j-1] + B[i-1][j] - B[i-1][j-1] + A[i][j];
                  }
                }
              }
            }
            else if (bi > 0) {
              #pragma omp task depend(in: deps[bi-1][bj]) depend(out: deps[bi][bj])
              {
                for (int i = bi*K; i < (bi+1)*K; i++) {
                  for (int j = bj*K; j < (bj+1)*K; j++) {
                    if (j == 0)
                      B[i][0] = B[i-1][0] + A[i][0];
                    else
                      B[i][j] = B[i][j-1] + B[i-1][j] - B[i-1][j-1] + A[i][j];
                  }
                }
              }
            }
            else if (bj > 0) {
              #pragma omp task depend(in: deps[bi][bj-1]) depend(out: deps[bi][bj])
              {
                for (int i = bi*K; i < (bi+1)*K; i++) {
                  for (int j = bj*K; j < (bj+1)*K; j++) {
                    if (i == 0)
                      B[0][j] = B[0][j-1] + A[0][j];
                    else
                      B[i][j] = B[i][j-1] + B[i-1][j] - B[i-1][j-1] + A[i][j];
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  auto tb1 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> block_time = tb1 - tb0;


  if (N < 20) {
    printf("Array B:\n");
    printArray(B);
  }
  std::cout << "Temps par (blocks): " << block_time.count() << " s\n";
  return 0;
}