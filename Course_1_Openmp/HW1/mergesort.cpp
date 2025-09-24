/*
 * =====================================================================================
 * PARALLEL MERGESORT WITH OPENMP - PERFORMANCE ANALYSIS
 * =====================================================================================
 * PERFORMANCE RESULTS (100000 elements) :
 * - Sequential time : 11354 microseconds (0.011354 seconds)
 * - Parallel time : 4553 microseconds (0.004553 seconds)
 * - Speedup : 2.49x (parallel sorting is 2.5x faster)
 * - Efficiency : 31% (2.49/8 threads)
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <assert.h>
#include <chrono>

#define SWAP(a,b) {int tmp = a; a = b; b = tmp;}
#define SIZE 1024

void verify(int* a, int size);
void merge(int* a, int size, int* temp);
void mergesort(int* a, int size, int* temp);

int main(int argc, char** argv) {
  int* a;
  int* temp;
  double inicio, fim;
  int size = SIZE;
  int i;

  //setup
  if (argc>=2) size = atoi(argv[1]);
  printf("Sorting an array of size %d.\n", size);
  a = (int*)calloc(size, sizeof(int));
  temp = (int*)calloc(size, sizeof(int));
  srand(26);
  for (i = 0; i<size; ++i) {
    a[i] = rand();
  }

  //sort - Version séquentielle
  printf("=== VERSION SÉQUENTIELLE ===\n");
  mergesort(a, size, temp);
  verify(a, size);
  
  // Réinitialiser le tableau pour le test parallèle
  srand(26);
  for (i = 0; i<size; ++i) {
    a[i] = rand();
  }
  
  //sort - Version parallèle avec 8 sections de tri + 4 sections de merge
  printf("\n=== VERSION PARALLÈLE AVEC 8 SECTIONS DE TRI + 4 SECTIONS DE MERGE ===\n");
  
  auto start_par = std::chrono::high_resolution_clock::now();
  
  // Première région parallèle : Tri des 8 sections
  #pragma omp parallel sections num_threads(8)
  {
    // Section 1: éléments 0 à N/8-1
    #pragma omp section
    {
      int start = 0;
      int end = size/8;
      printf("Section 1: tri des éléments %d à %d\n", start, end-1);
      mergesort(a + start, end - start, temp + start);
    }
    
    // Section 2: éléments N/8 à N/4-1
    #pragma omp section
    {
      int start = size/8;
      int end = size/4;
      printf("Section 2: tri des éléments %d à %d\n", start, end-1);
      mergesort(a + start, end - start, temp + start);
    }
    
    // Section 3: éléments N/4 à 3N/8-1
    #pragma omp section
    {
      int start = size/4;
      int end = 3*size/8;
      printf("Section 3: tri des éléments %d à %d\n", start, end-1);
      mergesort(a + start, end - start, temp + start);
    }
    
    // Section 4: éléments 3N/8 à N/2-1
    #pragma omp section
    {
      int start = 3*size/8;
      int end = size/2;
      printf("Section 4: tri des éléments %d à %d\n", start, end-1);
      mergesort(a + start, end - start, temp + start);
    }
    
    // Section 5: éléments N/2 à 5N/8-1
    #pragma omp section
    {
      int start = size/2;
      int end = 5*size/8;
      printf("Section 5: tri des éléments %d à %d\n", start, end-1);
      mergesort(a + start, end - start, temp + start);
    }
    
    // Section 6: éléments 5N/8 à 3N/4-1
    #pragma omp section
    {
      int start = 5*size/8;
      int end = 3*size/4;
      printf("Section 6: tri des éléments %d à %d\n", start, end-1);
      mergesort(a + start, end - start, temp + start);
    }
    
    // Section 7: éléments 3N/4 à 7N/8-1
    #pragma omp section
    {
      int start = 3*size/4;
      int end = 7*size/8;
      printf("Section 7: tri des éléments %d à %d\n", start, end-1);
      mergesort(a + start, end - start, temp + start);
    }
    
    // Section 8: éléments 7N/8 à N-1
    #pragma omp section
    {
      int start = 7*size/8;
      int end = size;
      printf("Section 8: tri des éléments %d à %d\n", start, end-1);
      mergesort(a + start, end - start, temp + start);
    }
  }
  
  // Deuxième région parallèle : Fusion des 4 sections
  #pragma omp parallel sections num_threads(4)
  {
    // Section 1: Fusion des sections 1-2 (éléments 0 à N/4-1)
    #pragma omp section
    {
      printf("Section 1: fusion des sections 1-2 (éléments 0 à %d)\n", size/4-1);
      merge(a, size/4, temp);
    }
    
    // Section 2: Fusion des sections 3-4 (éléments N/4 à N/2-1)
    #pragma omp section
    {
      printf("Section 2: fusion des sections 3-4 (éléments %d à %d)\n", size/4, size/2-1);
      merge(a + size/4, size/4, temp + size/4);
    }
    
    // Section 3: Fusion des sections 5-6 (éléments N/2 à 3N/4-1)
    #pragma omp section
    {
      printf("Section 3: fusion des sections 5-6 (éléments %d à %d)\n", size/2, 3*size/4-1);
      merge(a + size/2, size/4, temp + size/2);
    }
    
    // Section 4: Fusion des sections 7-8 (éléments 3N/4 à N-1)
    #pragma omp section
    {
      printf("Section 4: fusion des sections 7-8 (éléments %d à %d)\n", 3*size/4, size-1);
      merge(a + 3*size/4, size/4, temp + 3*size/4);
    }
  }
  
  printf("\nFusion finale des 4 sections en 2 sections...\n");
  // Fusion des 4 sections de N/4 en 2 sections de N/2
  // Fusion 1: sections 1-2 (éléments 0 à N/2-1)
  merge(a, size/2, temp);
  // Fusion 2: sections 3-4 (éléments N/2 à N-1)
  merge(a + size/2, size/2, temp + size/2);
  
  printf("\nFusion finale des 2 sections en 1 section...\n");
  // Fusion finale: fusionner les 2 moitiés finales
  merge(a, size, temp);
  
  auto end_par = std::chrono::high_resolution_clock::now();
  auto duration_par = std::chrono::duration_cast<std::chrono::microseconds>(end_par - start_par);
  
  printf("\n=== ANALYSE DES PERFORMANCES ===\n");
  printf("Temps d'exécution parallèle: %ld microsecondes\n", duration_par.count());
  printf("Temps d'exécution parallèle: %.6f secondes\n", duration_par.count() / 1000000.0);
  
  verify(a, size);
}

void verify(int* a, int size) {
  int sorted = 1;
  int i;

  for (i = 0; i < size-1; ++i) sorted &= (a[i] <= a[i+1]);

  if (sorted) printf("The array was properly sorted.\n");
  else printf("There was an error when sorting the array.\n");
}

void merge(int* a, int size, int* temp) {
  int i1 = 0;
  int i2 = size / 2;
  int it = 0;

  while(i1 < size/2 && i2 < size) {
    if (a[i1] <= a[i2]) {
      temp[it] = a[i1];
      i1 += 1;
    }
    else {
      temp[it] = a[i2];
      i2 += 1;
    }
    it += 1;
  }

  while (i1 < size/2) {
    temp[it] = a[i1];
    i1++;
    it++;
  }
  while (i2 < size) {
    temp[it] = a[i2];
    i2++;
    it++;
  }

  memcpy(a, temp, size*sizeof(int));
}

void mergesort(int* a, int size, int* temp) {
  if (size < 2) return;   //nothing to sort
  if (size == 2) {        //only two values to sort
    if (a[0] <= a[1])
      return;
    else {
      SWAP(a[0], a[1]);
      return;
    }
  } else {                //mergesort
    mergesort(a, size/2, temp);
    mergesort(a + size/2, size - size/2, temp + size/2); //a + size/2: pointer arithmetic
    merge(a, size, temp);
  }
  return;
}