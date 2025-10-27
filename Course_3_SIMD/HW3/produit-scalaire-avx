/**
  * e) The methodo with FMA is faster than simple multiplication then addition.
    g) yes it's accelerate. we had T = 6.10^(-7) et now 4.10^(-7)
  */

  #include <immintrin.h>
  #include <iostream>
  #include <iomanip>
  #include <chrono>
  #include <cstdlib>
  
  #define NREPET 1024
  
  int main(int argc, char **argv)
  {
    if (argc < 2) {
      std::cout << "Utilisation: \n  " << argv[0] << " [taille-de-tableau]\n";
      return 1;
    }
    int dim = std::atoi(argv[1]);
    if (dim % 8) {
      std::cout << "La taille de tableau doit etre un multiple de 8.\n";
      return 1;
    }

    
  
    // Allouer les tableaux tab0 et tab1 de flottants de taille dim alignes par 32 octets, puis initialiser tab0[i]=i
    float* tab0 = (float*) _mm_malloc(dim * sizeof(float), 32);
    float* tab1 = (float*) _mm_malloc(dim * sizeof(float), 32);

    if (!tab0 || !tab1) {
        std::cerr << "Erreur d'allocation mémoire" << std::endl;
        return 1;
    }

    for (int i = 0; i < dim; i++){
        tab0[i] = (float) i;
    }
  
    for (int i = 0; i < dim; i++){
        tab1[i] = (float) 1;
    }

    // Faire le produit scalaire non-vectorise. On repete le calcul NREPET fois pour mieux mesurer le temps d'execution
    auto start = std::chrono::high_resolution_clock::now();
    for (int repet = 0; repet < NREPET; repet++) {
        float sum = 0 ;
      for (int i = 0; i < dim; i++){
        sum += tab0[i] * tab1[i];

      }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tempsSeq = end-start;
    std::cout << std::scientific << "Produit scalaire sans AVX: " << tempsSeq.count() / NREPET << "s" << std::endl;
  
    // Faire le produit scalaire vectorise AVX. On repete le calcul NREPET fois pour mieux mesurer le temps d'execution
    start = std::chrono::high_resolution_clock::now();
    for (int repet = 0; repet < NREPET; repet++) {
      __m256 sum_part = _mm256_setzero_ps();
      for (int i = 0; i < dim; i+=8){
        __m256 r1;
        r1 = _mm256_load_ps(&tab0[i]);
        __m256 r2;
        r2 = _mm256_load_ps(&tab1[i]);
        __m256 r3;
        r3 = _mm256_mul_ps(r1, r2);
        sum_part = _mm256_add_ps(sum_part, r3);
      }
      float* tab2 = (float*) _mm_malloc(8 * sizeof(float), 32);
      _mm256_store_ps(&tab2[0], sum_part);
      float sum = 0;
      for (int i = 0; i < 8; i++){
        sum += tab2[i];
      }
      _mm_free(tab2);
      

    }
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tempsAVX = end-start;
    std::cout << std::scientific << "Produit scalaire avec AVX: " << tempsAVX.count() / NREPET << "s" << std::endl;

    // Faire le produit scalaire vectorise AVX - loop unrolling by a factor 2. On repete le calcul NREPET fois pour mieux mesurer le temps d'execution
    start = std::chrono::high_resolution_clock::now();
    for (int repet = 0; repet < NREPET; repet++) {
      __m256 sum_part_1 = _mm256_setzero_ps();
      __m256 sum_part_2 = _mm256_setzero_ps();
      __m256 sum_part = _mm256_setzero_ps();
      for (int i = 0; i < dim; i+=16){
        __m256 r1;
        r1 = _mm256_load_ps(&tab0[i]);
        __m256 r1_2;
        r1_2 = _mm256_load_ps(&tab0[i+8]);
        __m256 r2;
        r2 = _mm256_load_ps(&tab1[i]);
        __m256 r2_2;
        r2_2 = _mm256_load_ps(&tab1[i+8]);
        __m256 r3;
        r3 = _mm256_mul_ps(r1, r2);
        __m256 r3_2;
        r3_2 = _mm256_mul_ps(r1_2, r2_2);
        sum_part_1 = _mm256_add_ps(sum_part_1, r3);
        sum_part_2 = _mm256_add_ps(sum_part_2, r3_2);
      }
      sum_part = _mm256_add_ps(sum_part_1, sum_part_2);
      float* tab2 = (float*) _mm_malloc(8 * sizeof(float), 32);
      _mm256_store_ps(&tab2[0], sum_part);
      float sum = 0;
      for (int i = 0; i < 8; i++){
        sum += tab2[i];
      }
      _mm_free(tab2);
      

    }
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tempsAVX2 = end-start;
    std::cout << std::scientific << "Produit scalaire avec AVX - loop unrolling by a factor 2: " << tempsAVX2.count() / NREPET << "s" << std::endl;

    // Faire le produit scalaire vectorise AVX - loop unrolling by a factor 4. On repete le calcul NREPET fois pour mieux mesurer le temps d'execution
    start = std::chrono::high_resolution_clock::now();
    for (int repet = 0; repet < NREPET; repet++) {
      __m256 sum_part_1 = _mm256_setzero_ps();
      __m256 sum_part_2 = _mm256_setzero_ps();
      __m256 sum_part_3 = _mm256_setzero_ps();
      __m256 sum_part_4 = _mm256_setzero_ps();
      __m256 sum_part_1_2 = _mm256_setzero_ps();
      __m256 sum_part_3_4 = _mm256_setzero_ps();
      __m256 sum_part = _mm256_setzero_ps();
      for (int i = 0; i < dim; i+=32){
        __m256 r1;
        r1 = _mm256_load_ps(&tab0[i]);
        __m256 r1_2;
        r1_2 = _mm256_load_ps(&tab0[i+8]);
        __m256 r1_3;
        r1_3 = _mm256_load_ps(&tab0[i+16]);
        __m256 r1_4;
        r1_4 = _mm256_load_ps(&tab0[i+24]);
        __m256 r2;
        r2 = _mm256_load_ps(&tab1[i]);
        __m256 r2_2;
        r2_2 = _mm256_load_ps(&tab1[i+8]);
        __m256 r2_3;
        r2_3 = _mm256_load_ps(&tab1[i+16]);
        __m256 r2_4;
        r2_4 = _mm256_load_ps(&tab1[i+24]);
        __m256 r3;
        r3 = _mm256_mul_ps(r1, r2);
        __m256 r3_2;
        r3_2 = _mm256_mul_ps(r1_2, r2_2);
        __m256 r3_3;
        r3_3 = _mm256_mul_ps(r1_3, r2_3);
        __m256 r3_4;
        r3_4 = _mm256_mul_ps(r1_4, r2_4);
        sum_part_1 = _mm256_add_ps(sum_part_1, r3);
        sum_part_2 = _mm256_add_ps(sum_part_2, r3_2);
        sum_part_3 = _mm256_add_ps(sum_part_3, r3_3);
        sum_part_4 = _mm256_add_ps(sum_part_4, r3_4);
      }
      sum_part_1_2 = _mm256_add_ps(sum_part_1, sum_part_2);
      sum_part_3_4 = _mm256_add_ps(sum_part_3, sum_part_4);
      sum_part = _mm256_add_ps(sum_part_1_2, sum_part_3_4);
      float* tab2 = (float*) _mm_malloc(8 * sizeof(float), 32);
      _mm256_store_ps(&tab2[0], sum_part);
      float sum = 0;
      for (int i = 0; i < 8; i++){
        sum += tab2[i];
      }
      _mm_free(tab2);
      

    }
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tempsAVX4 = end-start;
    std::cout << std::scientific << "Produit scalaire avec AVX - loop unrolling by a factor 4: " << tempsAVX4.count() / NREPET << "s" << std::endl;
  
  
    // Produit scalaire vectorise AVX FMA. On repete le calcul NREPET fois pour mieux mesurer le temps d'execution
    start = std::chrono::high_resolution_clock::now();
    for (int repet = 0; repet < NREPET; repet++) {
      __m256 sum_part = _mm256_setzero_ps();
      for (int i = 0; i < dim; i+=8){
        __m256 r1;
        r1 = _mm256_load_ps(&tab0[i]);
        __m256 r2;
        r2 = _mm256_load_ps(&tab1[i]);
        sum_part = _mm256_fmadd_ps(r1, r2, sum_part);
      }
      float* tab2 = (float*) _mm_malloc(8 * sizeof(float), 32);
      _mm256_store_ps(&tab2[0], sum_part);
      float sum = 0;
      for (int i = 0; i < 8; i++){
        sum += tab2[i];
      }
      _mm_free(tab2);

    }
    
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tempsParAVXFMA = end-start;
    std::cout << std::scientific << "Produit scalaire avec AVX FMA: " << tempsParAVXFMA.count() / NREPET << "s" << std::endl;
  
    // Produit scalaire vectorise AVX FMA et deroulement. On repete le calcul NREPET fois pour mieux mesurer le temps
    start = std::chrono::high_resolution_clock::now();
    for (int repet = 0; repet < NREPET; repet++) {
      // A FAIRE ...
    }
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tempsParAVXFMADeroule = end-start;
    std::cout << std::scientific << "Produit scalaire avec AVX FMA deroulement: " << tempsParAVXFMADeroule.count() /
      NREPET << "s" << std::endl;
  
    // Desallouer les tableaux tab0 et tab1
    // A FAIRE ...

    _mm_free(tab0);
    _mm_free(tab1);
  
    return 0;
  }