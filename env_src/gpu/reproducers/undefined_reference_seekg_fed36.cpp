#include <fstream>
int main()
{
      std::ifstream fichier("toto"); // Calcul de l'espace disque pris par le fichier XYZ du probleme courant
      fichier.seekg(0L, std::ios_base::end);
return 1;
}
