ier #!/bin/bash
# Script de configuration pour OpenMP sur macOS
# Exécuter avec: source setup_openmp.sh

echo "Configuration de l'alias g++-openmp..."

# Créer l'alias pour cette session
alias g++-openmp="g++ -Xpreprocessor -fopenmp -I/opt/homebrew/Cellar/libomp/21.1.1/include -L/opt/homebrew/Cellar/libomp/21.1.1/lib -lomp"

echo "✅ Alias g++-openmp créé !"
echo "Vous pouvez maintenant compiler avec: g++-openmp votre_fichier.cpp -o executable"
echo ""
echo "Pour rendre cet alias permanent, ajoutez cette ligne à votre ~/.zshrc :"
echo "alias g++-openmp=\"g++ -Xpreprocessor -fopenmp -I/opt/homebrew/Cellar/libomp/21.1.1/include -L/opt/homebrew/Cellar/libomp/21.1.1/lib -lomp\""

