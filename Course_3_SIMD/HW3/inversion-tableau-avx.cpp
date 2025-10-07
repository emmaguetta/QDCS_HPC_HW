/**
  * Copie d'un tableau dans un autre avec les intrinseques AVX.
  * A compiler avec les drapeaux -O2 -mavx2.
  */

  #include <immintrin.h>
  #include <iostream>
  #include <iomanip>
  #include <chrono>
  #include <cstdlib>
  
  #define NREPET 1023
  
  void afficherUsage()
  { 
    printf("Usage: ./inversion-tableau-avx [taille-du-tableau]\n");
  }
  
  int main(int argc, char **argv)
  {
    int dim = 32;
    
    
    // Allouer et initialiser tableau tab de taille dim aligne par 32 octets
    float* tab = (float*) _mm_malloc(dim * sizeof(float), 32);

    for (int i = 0; i < dim; i++) {
        tab[i] = (float)i;   // ou toute autre valeur
    }

    printf("[ ");
    for (int i = 0; i < dim; i++) {
        printf("%.0f ", tab[i]); // affiche sans décimales
    }
    printf("]\n");
    
    

    
    // Inverser le tableau en place~(c'est a dire sans utiliser un deuxieme tableau auxiliaire) sans vectorisation
    // On repete NREPET fois pour mieux mesurer le temps d'execution
    auto start = std::chrono::high_resolution_clock::now();
    for (int repet = 0; repet < NREPET; repet++) {
      for (int i = 0; i < dim/2; i ++){
        float temp = tab[i];
        tab[i] = tab[dim - i - 1];
        tab[dim - i - 1]  = temp;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diffSeq = end-start;
    std::cout << std::scientific << "Inversion sans AVX: " << diffSeq.count() / NREPET << "s" << std::endl;

    printf("[ ");
    for (int i = 0; i < dim; i++) {
        printf("%.0f ", tab[i]); // affiche sans décimales
    }
    printf("]\n");


    for (int i = 0; i < dim; i++) {
        tab[i] = (float)i;   // ou toute autre valeur
    }

    
    
  
    // Inverser le tableau en place avec AVX~(c'est a dire sans utiliser un deuxieme tableau auxiliaire)
    // On repete NREPET fois pour mieux mesurer le temps d'execution
    if (dim % 16 != 0) {
        std::cerr << "Erreur: dim doit être un multiple de 16 pour l'inversion AVX." << std::endl;
        return 1;
    }

    start = std::chrono::high_resolution_clock::now();
    __m256i idx = _mm256_setr_epi32(7, 6, 5, 4, 3, 2, 1, 0);
    for (int repet = 0; repet < NREPET; repet++) {
        for (int i = 0; i < dim / 2; i+=8){
        __m256 r1 = _mm256_load_ps(&tab[i]);
        __m256 r2 = _mm256_load_ps(&tab[dim - i - 8]);        
        r1 = _mm256_permutevar8x32_ps(r1, idx);
        r2 = _mm256_permutevar8x32_ps(r2, idx);
        _mm256_store_ps(&tab[dim - i - 8],r1);
        _mm256_store_ps(&tab[i],r2 );
        }
    }
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diffPar = end-start;
    std::cout << std::scientific << "Inversion avec AVX: " << diffPar.count() / NREPET << "s" << std::endl;

    printf("[ ");
    for (int i = 0; i < dim; i++) {
        printf("%.0f ", tab[i]); // affiche sans décimales
    }
    printf("]\n");


    // Afficher l'acceleration et l'efficacite
    // A FAIRE ...
  
    // Desallouer le tableau tab
    // A FAIRE ...

    _mm_free(tab);
  
    return 0;
  }