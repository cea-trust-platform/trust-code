/****************************************************************************
* Copyright (c) 2022, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

#include <Synonyme_info.h>
#include <Noms.h>

// B.Mathieu, 08/2004
//  Le processus d'initialisation de ces membres statiques est tres
//  important : il faut absolument qu'ils soient initialises AVANT
//  le premier appel au constructeur Synonyme_info::Synonyme_info(...).
//  Or ce constructeur est appele pour initialiser le membre statique info_obj
//  de tous les Objet_U.
//  Risque de "static initialization ordre fiasco"
//    (voir http://www.parashift.com/c++-faq-lite/ctors.html   [10.11])
//  Pour l'instant c'est ok parce qu'on initialise avec une constante...

// Tableau de pointeurs sur les synonymes enregistres lors de la construction
// des Synonyme_info. Si plusieurs synonymes on le meme nom (Synonyme_info::n), alors
// on n'en enregistre qu'un seul dans Synonyme_info::les_synonymes.
const Synonyme_info** Synonyme_info::les_synonymes=0;
// Pour chaque elements du tableau "les_synonymes", ce nombre vaut 1 si le
// nom du synonyme est commun a plusieurs synonymes, 0 sinon.
// Voir "ajouter_synonyme"

int Synonyme_info::nb_classes=0;
int Synonyme_info::les_synonymes_memsize=0;

static inline char char_uppercase(char c)
{
  if (c >= 'a' && c <= 'z')
    c += 'A' - 'a';
  return c;
}

static inline int strcmp_uppercase(const char *n1, const char *n2)
{
  int i = 0;
  unsigned char c1, c2;
  int delta;
  do
    {
      c1 = (unsigned char) char_uppercase(n1[i]);
      c2 = (unsigned char) char_uppercase(n2[i]);
      delta = c1 - c2;
      i++;
    }
  while ((delta == 0) && (c1 != 0) && (c2 != 0));
  return delta;
}
// GF pour liberer correctement la memoire il faut au moins detruire le nom
Synonyme_info::~Synonyme_info()
{
  retirer_synonyme(nom());
  if ((nb_classes==0) && (les_synonymes_memsize!=0))
    {
      delete [] les_synonymes;
      les_synonymes_memsize=0;
    }
}

// Description:
//  Recherche le synonyme de nom "nom" dans la liste de synonymes enregistres
//  par une recherche binaire.
//  On compare les chaines converties en majuscules.
//  On range dans "index" l'indice du synonyme, s'il a ete trouve
//  et sinon l'indice du synonyme juste apres (dans ce cas, on a
//   les_synonymes[index-1]->n < nom < les_synonymes[index]->n )
//  Si le synonyme a ete trouve on renvoie 1, sinon 0.
int Synonyme_info::search_synonyme_info_name(const char *nom, int& index)
{
  assert(nom != 0);
  // [imin..imax] est l'intervalle ou se trouve l'index recherche
  int imin = 0;
  int imax = nb_classes;
  while (imax > imin)
    {
      // On a toujours milieu < imax
      int milieu = (imin + imax) / 2;
      int comparaison = strcmp_uppercase(nom, les_synonymes[milieu]->n);
      if (comparaison == 0)
        {
          index = milieu;
          return 1;
        }
      if (comparaison < 0)
        {
          // nom < les_synonymes[milieu]
          // l'index recherche est donc inferieur ou egal a "milieu"
          imax = milieu;
        }
      else
        {
          // nom > les_synonymes[milieu]
          // l'index recherche est donc strictement superieur a "milieu"
          imin = milieu + 1;
        }
    }
  index = imax;
  return 0;
}
// Description:
//     Constructeur par un nom, un tableau de meres.
// Precondition:
// Parametre: const char* nom
//    Signification: le nom du synonyme a creer
//    Valeurs par defaut:
//    Contraintes: nom != null
//    Acces: entree
// Parametre: int nb_base
//    Signification: le nombre de meres dans le tableau bases[]
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: const Synonyme_info* bases[]
//    Signification: le tableau specifiant les synonymes de bases (meres) du synonyme a creer
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
//    Sort en erreur si le nom n'est pas defini (null)
// Effets de bord:
// Postcondition:
Synonyme_info::Synonyme_info(const char* un_nom, const char* org_name) :
  n(un_nom),org(org_name)
{
  if((un_nom == 0)||(org_name==0))
    {
      Cerr << "Synonyme_info::Synonyme_info(const char* nom,Objet_U* (*f)()...)\n";
      Cerr << " Error : nom==0" << finl;
      assert(0);
      Process::exit();
    }
  ajouter_synonyme(*this);
}

// Description:
//  Methode statique appelee par les constructeurs de Synonyme_info pour
//  ajouter un nouveau synonyme a la liste des synonymes enregistres.
//  Verifie que le nom du synonyme n'existe pas encore.
void Synonyme_info::retirer_synonyme(const char* nom)
{
  // On cherche ou mettre le synonyme dans le tableau :
  int index;
  int existe_deja = search_synonyme_info_name(nom, index);
  if (!existe_deja)
    {
      Cerr<<"A synonym is suppressed whereas it doesn't exist !!!!!"<<finl;
      Process::exit();
    }
  else
    {
      // Ajout du synonyme dans le tableau a l'indice "index":
      for (int i = index; i < nb_classes-1; i++)
        les_synonymes[i] = les_synonymes[i+1];
      nb_classes--;
    }
}

// Description:
//  Methode statique appelee par les constructeurs de Synonyme_info pour
//  ajouter un nouveau synonyme a la liste des synonymes enregistres.
//  Verifie que le nom du synonyme n'existe pas encore.
void Synonyme_info::ajouter_synonyme(const Synonyme_info& synonyme_info)
{
  // Verifie qu'il y a assez de place dans le tableau :
  if (les_synonymes_memsize <= nb_classes + 1)
    {
      static const int INCREMENT = 512;
      // Plus assez de place dans le tableau: on redimensionne.
      les_synonymes_memsize += INCREMENT;
      const Synonyme_info** nouveau = new const Synonyme_info*[les_synonymes_memsize];
      for (int i = 0; i < nb_classes; i++)
        nouveau[i] = les_synonymes[i];
      delete[] les_synonymes;
      les_synonymes = nouveau;
    }

  // On cherche ou mettre le synonyme dans le tableau :
  int index;
  int existe_deja=Type_info::est_un_type(synonyme_info.n);
  if (existe_deja)
    {
      Cerr<<" The synonym "<<synonyme_info.n<<" exists as a class which is forbidden !!!!"<<finl;
      Process::exit();
    }
  existe_deja = search_synonyme_info_name(synonyme_info.n, index);
  if (existe_deja)
    {
      Cerr<<" Synonym "<<synonyme_info.n<<" already exists, which is forbidden!!!!"<<finl;
      Process::exit();
    }
  else
    {
      // Ajout du synonyme dans le tableau a l'indice "index":
      for (int i = nb_classes; i > index; i--)
        les_synonymes[i] = les_synonymes[i-1];
      les_synonymes[index] = &synonyme_info;

      nb_classes++;
    }
}

// Description:
//     Ecriture de toute la hierarchie du synonyme considere sur un flux de sortie
// Precondition:
// Parametre: Sortie& os
//    Signification: flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& Synonyme_info::hierarchie(Sortie& os)
{
  os << "There is " << nb_classes << " synomyms:" << finl;
  int i= nb_classes;
  while(i--)
    os << les_synonymes[i]->nom() << " <=> "<< les_synonymes[i]->org_name_()<<finl;
  return os << finl<<flush;
}

// Description:
//     Test d'existence d'une classe du synonyme indique
//     si il existe une classe T dont le Synonyme_info a
//     le nom nom, alors est_un_synonyme renvoie 1
//     renvoie le pointeur nul sinon.
// Precondition:
// Parametre: const char* nom
//    Signification: chaine de caractere associee a un synonyme
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: code de retour (0 ou  1)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Synonyme_info::est_un_synonyme(const char* nom)
{
  const Synonyme_info * synonyme = synonyme_info_from_name(nom);
  return (synonyme != 0);
}



// Description:
//  Methode statique qui renvoie un pointeur vers le Synonyme_info
//  dont le nom est "synonyme_name". Si synonyme_name n'est pas un synonyme,
//  renvoie un pointeur nul.
const Synonyme_info * Synonyme_info::synonyme_info_from_name(const char * synonyme_name)
{
  const Synonyme_info * synonyme_info = 0;
  if (synonyme_name != 0)
    {
      int index;
      if (search_synonyme_info_name(synonyme_name, index))
        synonyme_info = les_synonymes[index];
    }
  return synonyme_info;
}

