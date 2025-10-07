/**
  * Copie d'un tableau dans un autre avec les intrinseques AVX.
  * A compiler avec les drapeaux -O2 -mavx2.
  */

  #include <immintrin.h>
  #include <iostream>
  #include <iomanip>
  #include <chrono>
  #include <cstdlib>
  
  #define NREPET 1024
  
  void afficherUsage()
  { 
    printf("Usage: ./copier-vecteur-avx [taille-du-tableau]\n");
  }
  
  int main(int argc, char **argv)
  {
    if (argc < 2) { 
      afficherUsage();
      return 1;
    }
    int dim = std::atoi(argv[1]);
  
    // Allouer et initialiser deux tableaux de flottants de taille dim alignes par 32 octets
    float* tab0 = (float*) _mm_malloc(dim * sizeof(float), 32);
    float* tab1 = (float*) _mm_malloc(dim * sizeof(float), 32);

    if (!tab0 || !tab1) {
        std::cerr << "Erreur d'allocation mémoire" << std::endl;
        return 1;
    }

    for (int i = 0; i < dim; i++){
        tab0[i] = (float) i;
    }


    // A FAIRE ...
    
    // Copier tab0 dans tab1 de manière scalaire~(code non-vectorise).
    // On repete NREPET fois pour mieux mesurer le temps d'execution
    auto start = std::chrono::high_resolution_clock::now();
    for (int repet = 0; repet < NREPET; repet++) {
      for (int i = 0; i < dim; i++){
        tab1[i] = tab0[i];
      }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diffSeq = end-start;
    std::cout << std::scientific << "Copier sans AVX: " << diffSeq.count() / NREPET << "s" << std::endl;
  
    // Copier tab0 dans tab1 de maniere vectorisee avec AVX
    start = std::chrono::high_resolution_clock::now();
    for (int repet = 0; repet < NREPET; repet++) {
      for (int i = 0; i <= dim - 8; i +=8){
        __m256 vec = _mm256_load_ps(&tab0[i]);
        _mm256_store_ps(&tab1[i], vec);
      }
    }
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diffPar = end-start;
    std::cout << std::scientific << "Copier avec AVX: " << diffPar.count() / NREPET << "s" << std::endl;
    // Afficher l'acceleration et l'efficacite
    double tSeq = diffSeq.count() / NREPET;
    double tPar = diffPar.count() / NREPET;
    double speedupAVX = tSeq / tPar;
    double efficiencyAVX = speedupAVX / 8.0;  // 8 floats en parallèle
    std::cout << "Speedup AVX = " << speedupAVX
            << "  Efficacité AVX = " << efficiencyAVX << std::endl;
    // A FAIRE ...
  
    // Copier tab0 dans tab1 de maniere vectorisee avec AVX et deroulement de facteur 4
    start = std::chrono::high_resolution_clock::now();
    for (int repet = 0; repet < NREPET; repet++) {
        for (int i = 0; i <= dim-32; i += 32) {
            __m256 v0 = _mm256_load_ps(&tab0[i]);
            __m256 v1 = _mm256_load_ps(&tab0[i+8]);
            __m256 v2 = _mm256_load_ps(&tab0[i+16]);
            __m256 v3 = _mm256_load_ps(&tab0[i+24]);
        
            _mm256_store_ps(&tab1[i],   v0);
            _mm256_store_ps(&tab1[i+8], v1);
            _mm256_store_ps(&tab1[i+16], v2);
            _mm256_store_ps(&tab1[i+24], v3);
        }
    }
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diffParDeroule = end-start;
    std::cout << std::scientific << "Copier avec AVX et deroulement: " << diffParDeroule.count() / NREPET << "s" << std::endl;
    // Afficher l'acceleration et l'efficacite
    // A FAIRE ...
    double tParDeroule = diffParDeroule.count() / NREPET;
    double speedupUnrolled = tSeq / tParDeroule;
    double efficiencyUnrolled = speedupUnrolled / 32.0;  // 32 floats en parallèle
    std::cout << "Speedup AVX+deroulement = " << speedupUnrolled
          << "  Efficacité AVX+deroulement = " << efficiencyUnrolled << std::endl;
    // Desallouer les tableaux tab0 et tab1
    // A FAIRE ...
  
    _mm_free(tab0);
    _mm_free(tab1);

    return 0;
  }