#!/bin/bash

echo "=== Test de performance Mergesort ==="
echo "Comparaison version séquentielle vs parallèle"
echo ""

# Compiler les deux versions
echo "Compilation..."
g++-openmp mergesort_sequential.cpp -o mergesort_sequential
g++-openmp mergesort.cpp -o mergesort

if [ $? -ne 0 ]; then
    echo "Erreur de compilation. Arrêt du script."
    exit 1
fi

echo "✅ Compilation réussie"
echo ""

# Tester avec différentes tailles
sizes=(1000 10000 100000 1000000)

for size in "${sizes[@]}"
do
    echo "--- Test avec $size éléments ---"
    
    # Version séquentielle
    echo "Version séquentielle :"
    TIME_SEQ=$(./mergesort_sequential $size 2>&1 | grep "Temps d'exécution séquentiel:" | awk '{print $5}')
    echo "Temps: ${TIME_SEQ} microsecondes"
    
    # Version parallèle
    echo "Version parallèle :"
    TIME_PAR=$(./mergesort $size 2>&1 | grep "Temps parallèle:" | awk '{print $3}')
    echo "Temps: ${TIME_PAR} microsecondes"
    
    # Calcul du speedup
    if [ ! -z "$TIME_SEQ" ] && [ ! -z "$TIME_PAR" ]; then
        SPEEDUP=$(awk "BEGIN {printf \"%.2f\", ${TIME_SEQ} / ${TIME_PAR}}")
        echo "Speedup: $SPEEDUP"
        
        # Calcul de l'efficacité (8 threads)
        EFFICIENCY=$(awk "BEGIN {printf \"%.2f\", ${SPEEDUP} / 8}")
        echo "Efficacité: $EFFICIENCY (${EFFICIENCY}%)"
    fi
    
    echo ""
done

# Nettoyage
rm mergesort_sequential mergesort
echo "✅ Test terminé"