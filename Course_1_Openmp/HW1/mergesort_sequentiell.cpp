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

  //sort - Version séquentielle uniquement
  printf("=== VERSION SÉQUENTIELLE PURE ===\n");
  
  auto start = std::chrono::high_resolution_clock::now();
  mergesort(a, size, temp);
  auto end = std::chrono::high_resolution_clock::now();
  
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  printf("Temps d'exécution séquentiel: %ld microsecondes\n", duration.count());
  printf("Temps d'exécution séquentiel: %.6f secondes\n", duration.count() / 1000000.0);
  
  verify(a, size);
  
  // Nettoyage
  free(a);
  free(temp);
  
  return 0;
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