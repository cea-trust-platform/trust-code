/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Partitionneur_Tranche.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie/Decoupeur
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////
#include <Partitionneur_Tranche.h>
#include <Domaine.h>
#include <Reordonner_faces_periodiques.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Partitionneur_Tranche,"Partitionneur_Tranche",Partitionneur_base);

Partitionneur_Tranche::Partitionneur_Tranche()
{
}

// Description : La syntaxe est
//  { Tranches nx ny [ nz ] }
Entree& Partitionneur_Tranche::readOn(Entree& is)
{
  if (! ref_domaine_.non_nul())
    {
      Cerr << " Error: the domain has not been associated" << finl;
      exit();
    }

  Partitionneur_base::readOn(is);
  if (min_array(nb_tranches_)<1)
    {
      Cerr << "Error for the cutting domain tool (Tranche) specifications : " <<finl;
      Cerr<< " the number of slice must be greater than 0 for each direction. " << finl;
      exit();
    }
  return is;
}

Sortie& Partitionneur_Tranche::printOn(Sortie& os) const
{
  Cerr << "Partitionneur_Tranche::printOn invalid\n" << finl;
  exit();
  return os;
}

void Partitionneur_Tranche::set_param(Param& param)
{
  param.ajouter_arr_size_predefinie("tranches",&nb_tranches_,Param::REQUIRED);
}

// Description:
//  Premiere etape d'initialisation du partitionneur: on associe un domaine.
void Partitionneur_Tranche::associer_domaine(const Domaine& domaine)
{
  ref_domaine_ = domaine;
  nb_tranches_.resize_array(domaine.dimension);
  nb_tranches_ = -1;
}

// Description:
//  Deuxieme etape d'initialisation: on definit le nombre de tranches.
//  (on peut utiliser readOn a la place).
void Partitionneur_Tranche::initialiser(const ArrOfInt& nb_tranches)
{
  assert(ref_domaine_.non_nul());
  assert(nb_tranches.size_array() == nb_tranches_.size_array());
  assert(min_array(nb_tranches) > 0);

  nb_tranches_ = nb_tranches;
}

static const DoubleTab * static_qsort_DoubleTab_ptr;
static int            static_qsort_DoubleTab_colonne;

// Description: fonction outil pour tri quicksort
//  (comparaison de deux reels avec un index)
static True_int trier_index_compare_fct(const void *i_, const void *j_)
{
  const int i = *((int*)i_);
  const int j = *((int*)j_);
  const int k = static_qsort_DoubleTab_colonne;
  const DoubleTab& tab = *static_qsort_DoubleTab_ptr;
  const double x = tab(i,k);
  const double y = tab(j,k);
  True_int resu = 0;
  if (x > y)
    resu = 1;
  else if (x < y)
    resu = -1;
  return resu;
}

// Description: Fonction outil utilisee dans construire_partition.
//  On reordonne les valeurs du tableau index de sorte que les valeurs
//  tab(index[i], colonne) soient classees dans l'ordre croissant.
static void trier_index_colonne_i(const DoubleTab& tab,
                                  ArrOfInt& index,
                                  const int colonne)
{
  assert(colonne >= 0 && colonne < tab.dimension(1));

  static_qsort_DoubleTab_ptr = &tab;
  static_qsort_DoubleTab_colonne = colonne;
  int * index_ptr = index.addr();
  const int index_size = index.size_array();
  const int index_member_size = sizeof(int);

  qsort(index_ptr, index_size, index_member_size, trier_index_compare_fct);
}

// Description: Remplissage du tableau directions perio a partir des noms des bords
//  periodiques. Pour 0 <= i < Objet_U::dimension directions_perio[i] vaut 1 s'il
//  existe un bord periodique pour lequel le vecteur delta est dirige dans la direction i.
void Partitionneur_Tranche::chercher_direction_perio(const Zone& zone,
                                                     const Noms& liste_bords_perio,
                                                     ArrOfInt& directions_perio)
{
  Cerr << "Search of periodic directions of domain " << zone.domaine().le_nom() << finl;
  const int dim = Objet_U::dimension;
  directions_perio.resize_array(dim);
  directions_perio = 0;
  const int n = liste_bords_perio.size();
  for (int i = 0; i < n; i++)
    {
      int num_bord = 0;
      const Nom& nom_bord = liste_bords_perio[i];
      const int nb_bords = zone.nb_bords();
      while (num_bord < nb_bords)
        {
          if (zone.bord(num_bord).le_nom() == nom_bord)
            break;
          num_bord++;
        }
      if (num_bord == nb_bords)
        {
          Cerr << "Error in Partitionneur_Tranche::chercher_direction_perio\n"
               << " boundary not found : " << nom_bord << finl;
          exit();
        }
      ArrOfDouble delta;
      ArrOfDouble erreur;
      const double epsilon = Objet_U::precision_geom;
      Reordonner_faces_periodiques::check_faces_periodiques(zone.bord(num_bord), delta, erreur);
      int count = 0;
      for (int j = 0; j < dim; j++)
        {
          if (dabs(delta[j]) > epsilon)
            {
              directions_perio[j]++;
              Cerr << " Boundary : " << nom_bord << " periodic direction : " << j << finl;
              count++;
            }
        }
      if (count != 1)
        {
          Cerr << "Error in Partitionneur_Tranche::chercher_direction_perio\n"
               << " periodic direction not found for the boundary " << nom_bord
               << "\n Vector delta found between faces twin : " << delta
               << "\n with a maximum error : " << erreur << finl;
          exit();
        }
    }
  if (max_array(directions_perio) > 1)
    {
      Cerr << "Error in Partitionneur_Tranche::chercher_direction_perio\n"
           << " several boundaries have the same periodic direction" << finl;
      exit();
    }
}

void Partitionneur_Tranche::construire_partition(ArrOfInt& elem_part, int& nb_parts_tot) const
{
  assert(ref_domaine_.non_nul());
  assert(nb_tranches_[0] > 0);

  const Domaine& dom = ref_domaine_.valeur();
  const Zone& zone = dom.zone(0);
  const int nb_elem = zone.nb_elem();

  const int dim = dom.dimension;

  // Centre de gravite des elements:
  DoubleTab coord_g;
  // Index utilise pour trier les elements
  ArrOfInt index;

  // Pour chaque dimension d'espace, cette direction est-elle
  // une direction de periodicite
  ArrOfInt directions_perio;
  chercher_direction_perio(zone, liste_bords_periodiques_, directions_perio);

  Cerr << "Calculation of centers of gravity of the elements" << finl;
  zone.calculer_centres_gravite(coord_g);
  assert(coord_g.dimension(0) == nb_elem);
  assert(coord_g.dimension(1) == dim);

  Cerr << "Moving of centers of gravity" << finl;
  // Deplacement des coordonnees pour qu'il y ait une vraie relation d'ordre
  // dans chaque direction (en VDF, eviter les coordonnees identiques)
  {
    for (int i = 0; i < nb_elem; i++)
      {
        double x = coord_g(i,0);
        double y = coord_g(i,1);
        double z = (dim == 3) ? coord_g(i,2) : 0.;
        coord_g(i,0) = x + y*1e-6 + z*1e-12;
        coord_g(i,1) = y + x*1e-6 + z*1e-12;
        if (dim == 3)
          coord_g(i,2) = z + x*1e-6 + y*1e-12;
      }
  }

  // Nombre d'elements dans chaque partie
  //  (initialisation: une partie contenant nb_elem elements)
  ArrOfInt nb_elem_part(1);
  nb_elem_part= nb_elem;

  // Dans l'ordre croissant des parties, liste des elements
  // contenus dans la partie
  // (exemple:
  //  de 0..nb_elem_part[0]-1                (elements de la partie 0)
  //  de nb_elem_part[0]..nb_elem_part[0]+nb_elem_part[1]-1 (partie 1)
  // Initialisation: une seule grosse partie, tous les elements
  ArrOfInt listes_elem(nb_elem);
  for (int i = 0; i < nb_elem; i++)
    listes_elem[i] = i;

  ArrOfInt new_nb_elem_part;
  new_nb_elem_part.set_smart_resize(1);

  // Algorithme: on cree nb_tranches[0] tranches dans la direction x,
  //  chaque tranche compte le meme nombre d'elements du domaine.
  //  Puis on prend chacune de ces tranches et on la decoupe en nb_tranches[1]
  //  parties dans la direction y, elles aussi equilibrees.
  //  Enfin, on prend chacune des parties obtenues et on la coupe en nb_tranches[2]
  //  parties dans la direction z.
  // Attention: si le domaine n'est pas un parallelipipede, le decoupage
  //  n'est pas forcement cartesien:
  //    ------------------
  //    |       |        |
  //    |-------|        |
  //    |       |--------| <- les coins des tranches ne coincident pas forcement
  //    ------------------

  for (int direction = 0; direction < dim; direction++)
    {
      // Nombre de tranches a creer dans cette direction
      const int nb_tranches = nb_tranches_[direction];

      int index_debut_partie = 0;
      new_nb_elem_part.resize_array(0);
      const int nb_parts = nb_elem_part.size_array();

      Cerr << "The mesh contains " << nb_parts << " parts. ";
      Cerr << "Splitting in the direction " << direction << finl;

      // Boucle sur les parties deja creees. On divise chaque partie
      // en nb_tranches sous-parties:
      for (int part = 0; part < nb_parts; part++)
        {
          Cerr << " Splitting of subpart " << part << finl;
          // Extraction de la liste des elements de cette partie:
          //  on fait pointer le tableau index vers une sous-partie de listes_elem
          const int nb_elem_partie = nb_elem_part[part];
          assert(index_debut_partie >= 0 && index_debut_partie + nb_elem_partie <= nb_elem);
          index.ref_data(listes_elem.addr()+index_debut_partie, nb_elem_partie);
          // Tri des elements de cette partie dans l'ordre croissant
          // de la coordonnee (index pointe sur une partie de liste_elem, on trie
          // donc en realite une partie du tableau liste_elem)
          trier_index_colonne_i(coord_g, index, direction);

          // Si cette direction est periodique, permutation circulaire de
          //  nb_elem_partie/(nb_tranches*2) elements pour que les elements de l'extremite
          //  se retrouvent sur la meme partie que ceux de l'extremite opposee
          if (directions_perio[direction])
            {
              ArrOfInt copie(nb_elem_partie);
              copie.inject_array(index);
              int j = nb_elem_partie/(nb_tranches*2);
              for (int i = 0; i < nb_elem_partie; i++)
                {
                  index[j++] = copie[i];
                  if (j >= nb_elem_partie)
                    j = 0;
                }
            }
          // Construction d'une partition en nb_tranches parties, par
          // ordre croissant des coordonnees
          {
            int n = 0;
            for (int i = 0; i < nb_tranches; i++)
              {
                //const int new_n = nb_elem_partie * (i+1) / nb_tranches;
                // Rewritten cause nb_elem_partie * (i+1) > 2^31 for big meshes
                const int new_n = (int) ((double)nb_elem_partie * (i+1) / nb_tranches);
                new_nb_elem_part.append_array(new_n - n);
                n = new_n;
              }
          }
          index_debut_partie += nb_elem_partie;
        }

      nb_elem_part = new_nb_elem_part;
    }

  // Partition initiale du domaine: au depart tout dans 0
  elem_part.resize_array(nb_elem);
  elem_part = 0;
  // Construction de elem_part
  const int nb_parts = nb_elem_part.size_array();
  int index_fin = 0;
  int i = 0;
  for (int part = 0; part < nb_parts; part++)
    {
      index_fin += nb_elem_part[part];
      for (; i < index_fin; i++)
        {
          const int elem = listes_elem[i];
          elem_part[elem] = part;
        }
    }

  if (liste_bords_periodiques_.size() > 0)
    corriger_bords_avec_liste(dom, liste_bords_periodiques_, elem_part);

  Cerr << "Correction elem0 on processor 0" << finl;
  corriger_elem0_sur_proc0(elem_part);
}
