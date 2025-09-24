#include <cstdio>
#include <iostream>
#include "omp.h"

int main()
{
    // )
#pragma omp parallel default(none) num_threads(3)
    {
        int thid = omp_get_thread_num();
        int numth = omp_get_num_threads();
        printf("%d\n, %d\n", thid, numth);
    }

    // b)
    {
        int thid = omp_get_thread_num();
        int numth = omp_get_num_threads();
        if (thid == 0) {
            printf("Hello from thread %d\n", thid);
        }
    }
    
    printf("\n=== Test avec omp single ===\n");
    
    // b) Test avec omp single
#pragma omp parallel default(none) num_threads(3)
    {
        int thid = omp_get_thread_num();
        int numth = omp_get_num_threads();
        printf("Thread %d: ID=%d, Total threads=%d\n", thid, thid, numth);
        
        #pragma omp single
        {
            printf("Hello World from threadId=%d (omp single)\n", thid);
        }
        
        printf("Thread %d: Après single\n", thid);
    }
    
    printf("\n=== Test avec omp master ===\n");

    // b) Test avec omp master
#pragma omp parallel default(none) num_threads(3)
    {
        int thid = omp_get_thread_num();
        int numth = omp_get_num_threads();
        printf("Thread %d: ID=%d, Total threads=%d\n", thid, thid, numth);
        
        #pragma omp master
        {
            printf("Hello World from threadId=%d (omp master)\n", thid);
        }
        
        printf("Thread %d: Après master\n", thid);
    }

    printf("\n=== Partie c) - Test des 3 méthodes pour modifier le nombre de threads ===\n");
    
    // Méthode 1: Variable d'environnement OMP_NUM_THREADS
    printf("\n--- Méthode 1: Variable d'environnement OMP_NUM_THREADS ---\n");
    printf("Note: OMP_NUM_THREADS=2 (définie dans le terminal)\n");
    
#pragma omp parallel default(none)
    {
        int thid = omp_get_thread_num();
        int numth = omp_get_num_threads();
        printf("Thread %d: Total threads=%d (via OMP_NUM_THREADS)\n", thid, numth);
    }
    
    // Méthode 2: Fonction omp_set_num_threads()
    printf("\n--- Méthode 2: omp_set_num_threads(4) ---\n");
    omp_set_num_threads(4);
    
#pragma omp parallel default(none)
    {
        int thid = omp_get_thread_num();
        int numth = omp_get_num_threads();
        printf("Thread %d: Total threads=%d (via omp_set_num_threads)\n", thid, numth);
    }
    
    // Méthode 3: Clause num_threads() dans la directive
    printf("\n--- Méthode 3: num_threads(5) dans la directive ---\n");
    
#pragma omp parallel default(none) num_threads(5)
    {
        int thid = omp_get_thread_num();
        int numth = omp_get_num_threads();
        printf("Thread %d: Total threads=%d (via num_threads clause)\n", thid, numth);
    }
    
    // Test de priorité: omp_set_num_threads() vs num_threads()
    printf("\n--- Test de priorité: omp_set_num_threads(3) vs num_threads(6) ---\n");
    omp_set_num_threads(3);
    
#pragma omp parallel default(none) num_threads(6)
    {
        int thid = omp_get_thread_num();
        int numth = omp_get_num_threads();
        printf("Thread %d: Total threads=%d (num_threads clause gagne)\n", thid, numth);
    }

    return 0;
}