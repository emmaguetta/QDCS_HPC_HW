/* b. The i→k→j version is faster because it accesses the matrices in the same order they are stored in memory (by rows), allowing the processor to reuse cached data efficiently.
In contrast, the i→j→k version accesses matrix B by columns, causing many cache misses due to non-contiguous memory access.
As a result, the i→k→j loop order achieves much better cache locality and runs significantly faster. 
c. The version with single tiling runs faster than the one without tiling on very big matrixs.
g. We got (for N = 1024), T = 8.923468e+00s for the first naive version and T = 1.15e-01s for the AVX version with tiling. We got Speedup S = 77.6.
For N = 512, P = 4682.31Mflops/s
For N = 1024, P = 18675.77Mflops/s
for N >= 2048, unfortunatly my online compiler can't compile it. I got Request failed: gateway timeout
Since the code was executed on an online compiler with an unknown hardware backend, we assume a typical 3.0 GHz CPU supporting AVX2 and FMA, giving a theoretical peak of about 32 × 3.0 = 96 GFLOPS per core in single precision. The optimized version reaches approximately 20 % of the theoretical peak performance (assuming 96 GFLOPS per core).
*/

#include <iostream>
#include <iomanip>
#include <vector>
#include <cstring>
#include <cstdlib>
#include "immintrin.h"
#include <chrono>
#include "omp.h"

#define NREPEAT 10

void printUsage(int argc, char **argv)
{
  printf("Usage: %s N\n", argv[0]);
  printf("Example: %s 1024\n", argv[0]);
}

void verify(const float *A, const float *B, const float *C, int N)
{
  int correct = 1;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (C[i * N + j] != N) {
        printf("C(%d, %d) = %f is incorrect; C(%d, %d) should be %f\n", i, j, C[i * N + j], i, j, N);
        correct = 0;
        break;
      }
    }
  }
  if (correct) {
    printf("The result is correct!\n\n");
  } else {
    printf("The result is not correct!\n\n");
  }
}

inline void loadTile(__m256 tile[8], float *addr, int N)
{
  // Load the 8x8 subtile from memory whose upper left element is addr[0] in an NxN matrix
  // TODO / A FAIRE ...
  
    tile[0] = _mm256_load_ps(addr + 0 * N);
    tile[1] = _mm256_load_ps(addr + 1 * N);
    tile[2] = _mm256_load_ps(addr + 2 * N);
    tile[3] = _mm256_load_ps(addr + 3 * N);
    tile[4] = _mm256_load_ps(addr + 4 * N);
    tile[5] = _mm256_load_ps(addr + 5 * N);
    tile[6] = _mm256_load_ps(addr + 6 * N);
    tile[7] = _mm256_load_ps(addr + 7 * N);
  
}

inline void storeTile(__m256 tile[8], float *addr, int N)
{
  _mm256_store_ps(addr + 0 * N, tile[0]);
  _mm256_store_ps(addr + 1 * N, tile[1]);
  _mm256_store_ps(addr + 2 * N, tile[2]);
  _mm256_store_ps(addr + 3 * N, tile[3]);
  _mm256_store_ps(addr + 4 * N, tile[4]);
  _mm256_store_ps(addr + 5 * N, tile[5]);
  _mm256_store_ps(addr + 6 * N, tile[6]);
  _mm256_store_ps(addr + 7 * N, tile[7]);
}

inline void multiplyTile(float *tA, float *tB, float *tC, __m256 atile[8], __m256 btile[8], __m256 ctile[8], int N)
{
  loadTile(btile, tB, N);
  loadTile(ctile, tC, N);
  for (int i = 0; i < 8; i++) {
    // Perform the multiplication Ctile(i, :) += Atile(i, k) * Btile(k, :) for all values k = 0...7 in an unrolled way
    // Effectuer la multiplication Ctile(i, :) += Atile(i, k) * Btile(k, :) pour toutes les valeurs de k = 0...7 de
    // manière déroulé sur les valeurs de k
    // TODO / A FAIRE ...
    __m256 c = ctile[i];
    float *aRow = tA + i * N;

    // Déroulage explicite k = 0..7 pour masquage de latence et limiter les deps
    __m256 a0 = _mm256_broadcast_ss(aRow + 0);
    c = _mm256_fmadd_ps(a0, btile[0], c);

    __m256 a1 = _mm256_broadcast_ss(aRow + 1);
    c = _mm256_fmadd_ps(a1, btile[1], c);

    __m256 a2 = _mm256_broadcast_ss(aRow + 2);
    c = _mm256_fmadd_ps(a2, btile[2], c);

    __m256 a3 = _mm256_broadcast_ss(aRow + 3);
    c = _mm256_fmadd_ps(a3, btile[3], c);

    __m256 a4 = _mm256_broadcast_ss(aRow + 4);
    c = _mm256_fmadd_ps(a4, btile[4], c);

    __m256 a5 = _mm256_broadcast_ss(aRow + 5);
    c = _mm256_fmadd_ps(a5, btile[5], c);

    __m256 a6 = _mm256_broadcast_ss(aRow + 6);
    c = _mm256_fmadd_ps(a6, btile[6], c);

    __m256 a7 = _mm256_broadcast_ss(aRow + 7);
    c = _mm256_fmadd_ps(a7, btile[7], c);

    ctile[i] = c;
  }
  storeTile(ctile, tC, N);
}

int main(int argc, char **argv)
{
  if (argc != 2) {
    printUsage(argc, argv);
    return 0;
  }
  int N = std::atoi(argv[1]);
  const int B1 = 32;
  const int B2 = 256;

  // Allocate and initialize the matrix A and vectors x, b
  // Allouer et initialiser la matrice A et matrices x, b
  float *A = (float *)_mm_malloc(N * N * sizeof(float), 32);
  float *B = (float *)_mm_malloc(N * N * sizeof(float), 32);
  float *C = (float *)_mm_malloc(N * N * sizeof(float), 32);
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      A[i * N + j] = 1.0f;
      B[i * N + j] = 1.0f;
      C[i * N + j] = 0.0f;
    }
  }

  // Sequential and scalar matrix-matrix multiplication code with loop order i->j->k
  // Code sequentiel et scalaire produit matrice-matrice avec l'ordre de boucles i->j->k
  {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; i ++){
      for (int j = 0; j < N; j ++){
        for (int k = 0; k < N; k ++){
          C[i * N + j] += A[i * N + k] * B[k * N + j];
        }
      }
    }

    std::chrono::duration<double> timeDiff = std::chrono::high_resolution_clock::now() - start;
    std::cout << std::scientific << "Sequential scalar matmat i->j->k took " << timeDiff.count() << "s." << std::endl;
    std::cout << std::fixed << std::setprecision(2) << "Performance: " << 2.0*N*N*(N-1) / ((1e6) * timeDiff.count()) <<
      "Mflops/s" << std::endl;
    verify(A, B, C, N);
  }

  // Sequential and scalar matrix-matrix multiplication code with loop order i->k->j
  // Code sequentiel et scalaire produit matrice-matrice avec l'ordre de boucles i->k->j
  {
    auto start = std::chrono::high_resolution_clock::now();
    for (int repeat = 0; repeat < NREPEAT; repeat++) {
      memset(&C[0], 0, N * N * sizeof(float));
      for (int i = 0; i < N; i ++){
        for (int k = 0; k < N; k ++){
          for (int j = 0; j < N; j ++){
            C[i * N + j] += A[i * N + k] * B[k * N + j];
          }
        }
      }

    }
    std::chrono::duration<double> timeDiff = (std::chrono::high_resolution_clock::now() - start) / NREPEAT;
    std::cout << std::scientific << "Sequential scalar matmat i->k->j took " << timeDiff.count() << "s." << std::endl;
    std::cout << std::fixed << std::setprecision(2) << "Performance: " << 2.0*N*N*(N-1) / ((1e6) * timeDiff.count()) <<
      "Mflops/s" << std::endl;
    verify(A, B, C, N);
  }

  // Sequential and scalar matrix-matrix multiplication code with loop order i->k->j and single level tiling
  // Code sequentiel et scalaire produit matrice-matrice avec l'ordre de boucles i->k->j et tuilage d'un niveau
  {
    auto start = std::chrono::high_resolution_clock::now();
    for (int repeat = 0; repeat < NREPEAT; repeat++) {
      memset(&C[0], 0, N * N * sizeof(float));
      // TODO / A FAIRE ...
      for (int i = 0; i < N; i += B1) {
        for (int k = 0; k < N; k += B1) {
          for (int j = 0; j < N; j += B1) {
            float *tA = &A[i * N + k];
            float *tB = &B[k * N + j];
            float *tC = &C[i * N + j];
            for (int i2 = 0; i2 < B1; i2++) {
              for (int k2 = 0; k2 < B1; k2++) {
                for (int j2 = 0; j2 < B1; j2++) {
                  tC[i2 * N + j2] += tA[i2 * N + k2] * tB[k2 * N + j2];
                }
              }
            }
          }
        }
      }
    }
    std::chrono::duration<double> timeDiff = (std::chrono::high_resolution_clock::now() - start) / NREPEAT;
    std::cout << std::scientific << "Single tile scalar matmat i->k->j took " << timeDiff.count() << "s." << std::endl;
    std::cout << std::fixed << std::setprecision(2) << "Performance: " << 2.0*N*N*(N-1) / ((1e6) * timeDiff.count()) <<
      "Mflops/s" << std::endl;
    verify(A, B, C, N);
  }


  // Sequential and scalar matrix-matrix multiplication code with loop order i->k->j and two level tiling
  // Code sequentiel et scalaire produit matrice-matrice avec l'ordre de boucles i->k->j et tuilage de deux niveaux
  {
    auto start = std::chrono::high_resolution_clock::now();
    for (int repeat = 0; repeat < NREPEAT; repeat++) {
      memset(&C[0], 0, N * N * sizeof(float));

      for (int i = 0; i < N; i += B2) {
        for (int k = 0; k < N; k += B2) {
          for (int j = 0; j < N; j += B2) {

            for (int ii = i; ii < i + B2 && ii < N; ii += B1) {
              for (int kk = k; kk < k + B2 && kk < N; kk += B1) {
                for (int jj = j; jj < j + B2 && jj < N; jj += B1) {

                  for (int i2 = 0; i2 < B1 && (ii + i2) < N; i2++) {
                    for (int k2 = 0; k2 < B1 && (kk + k2) < N; k2++) {
                      float aik = A[(ii + i2) * N + (kk + k2)];
                      for (int j2 = 0; j2 < B1 && (jj + j2) < N; j2++) {
                        C[(ii + i2) * N + (jj + j2)] +=
                          aik * B[(kk + k2) * N + (jj + j2)];
                      }
                    }
                  }

                }
              }
            }
          }
        }
      }
    }

    std::chrono::duration<double> timeDiff = (std::chrono::high_resolution_clock::now() - start) / NREPEAT;
    std::cout << std::scientific << "Double tile scalar matmat i->k->j took " << timeDiff.count() << "s." << std::endl;
    std::cout << std::fixed << std::setprecision(2) << "Performance: " << 2.0*N*N*(N-1) / ((1e6) * timeDiff.count()) <<
      "Mflops/s" << std::endl;
    verify(A, B, C, N);
  }


  // Vectorized matrix-matrix multiplication code with loop order i->k->j and two level tiling + AVX
  // Produit matrice-matrice vectorise avec l'ordre de boucles i->k->j et tuilage de deux niveaux + AVX
  {
    auto start = std::chrono::high_resolution_clock::now();
    for (int repeat = 0; repeat < NREPEAT; repeat++) {
      memset(&C[0], 0, N * N * sizeof(float));
      __m256 atile[8], btile[8], ctile[8];

      for (int i = 0; i < N; i += B2) {
        for (int k = 0; k < N; k += B2) {
          for (int j = 0; j < N; j += B2) {

            int iEndB2 = std::min(i + B2, N);
            int kEndB2 = std::min(k + B2, N);
            int jEndB2 = std::min(j + B2, N);

            for (int ii = i; ii < iEndB2; ii += B1) {
              for (int kk = k; kk < kEndB2; kk += B1) {
                for (int jj = j; jj < jEndB2; jj += B1) {

                  int iEndB1 = std::min(ii + B1, N);
                  int kEndB1 = std::min(kk + B1, N);
                  int jEndB1 = std::min(jj + B1, N);

                  // Parcours par blocs 8×8 à l’intérieur du bloc B1×B1
                  for (int i2 = ii; i2 + 7 < iEndB1; i2 += 8) {
                    for (int k2 = kk; k2 + 7 < kEndB1; k2 += 8) {
                      for (int j2 = jj; j2 + 7 < jEndB1; j2 += 8) {
                        float *tA = &A[i2 * N + k2];
                        float *tB = &B[k2 * N + j2];
                        float *tC = &C[i2 * N + j2];

                        // Atile/btile/ctile sont des tampons registres pour le noyau
                        multiplyTile(tA, tB, tC, atile, btile, ctile, N);
                      }
                    }
                  }

                }
              }
            }
          
          }
        }
      }
    
  }
    std::chrono::duration<double> timeDiff = (std::chrono::high_resolution_clock::now() - start) / NREPEAT;
    std::cout << std::scientific << "Double tile AVX matmat i->k->j took " << timeDiff.count() << "s." << std::endl;
    std::cout << std::fixed << std::setprecision(2) << "Performance: " << 2.0*N*N*(N-1) / ((1e6) * timeDiff.count()) <<
      "Mflops/s" << std::endl;
    verify(A, B, C, N);
  }


  // Task-parallel and vectorized matrix-matrix multiplication code with loop order i->k->j and two level tiling + AVX
  // Produit matrice-matrice vectorise et parallelise par taches avec l'ordre de boucles i->k->j et tuilage de deux
  // niveaux+AVX
  {
    auto start = std::chrono::high_resolution_clock::now();
    for (int repeat = 0; repeat < NREPEAT; repeat++) {
      memset(&C[0], 0, N * N * sizeof(float));
      // TODO / A FAIRE ...
    }
    std::chrono::duration<double> timeDiff = (std::chrono::high_resolution_clock::now() - start) / NREPEAT;
    std::cout << std::scientific << "Task parallel double tile AVX matmat i->k->j took " << timeDiff.count() << "s." << std::endl;
    std::cout << std::fixed << std::setprecision(2) << "Performance: " << 2.0*N*N*(N-1) / ((1e6) * timeDiff.count()) <<
      "Mflops/s" << std::endl;
    verify(A, B, C, N);
  }

  // Free matrices
  // Desallouer les matrices
  _mm_free(A);
  _mm_free(B);
  _mm_free(C);

  return 0;
}
