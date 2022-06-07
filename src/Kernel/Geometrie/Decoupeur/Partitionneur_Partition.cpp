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
#include <Partitionneur_Partition.h>
#include <Domaine.h>
#include <Reordonner_faces_periodiques.h>
#include <EFichier.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Partitionneur_Partition,"Partitionneur_Partition",Partitionneur_base);

Partitionneur_Partition::Partitionneur_Partition()
{
  dom_calcul_ = "";
}

// Description : La syntaxe est
//  { Domaine NOM_DOM }
Entree& Partitionneur_Partition::readOn(Entree& is)
{
  if (! ref_domaine_interpol_.non_nul())
    {
      Cerr << " Error: the domain has not been associated" << finl;
      exit();
    }

  Partitionneur_base::readOn(is);
  Cerr << " Reference domain : " << dom_calcul_ << finl;
  ref_domaine_calcul_ = ref_cast(Domaine, Interprete::objet(dom_calcul_));
  return is;
}

Sortie& Partitionneur_Partition::printOn(Sortie& os) const
{
  Cerr << "Partitionneur_Partition::printOn invalid\n" << finl;
  exit();
  return os;
}

void Partitionneur_Partition::set_param(Param& param)
{
  param.ajouter("domaine",&dom_calcul_,Param::REQUIRED);
}

// Description:
//  Premiere etape d'initialisation du partitionneur: on associe un domaine.
void Partitionneur_Partition::associer_domaine(const Domaine& domaine)
{
  ref_domaine_interpol_ = domaine;
}

// Description:
//  Deuxieme etape d'initialisation: on definit le nombre de tranches.
//  (on peut utiliser readOn a la place).
void Partitionneur_Partition::initialiser()
{
  assert(ref_domaine_interpol_.non_nul());
}

void Partitionneur_Partition::construire_partition(IntVect& elem_part, int& nb_parts_tot) const
{
  assert(ref_domaine_interpol_.non_nul());
  assert(ref_domaine_calcul_.non_nul());

  // Domaine dom_interpol
  const Domaine& dom_interpol = ref_domaine_interpol_.valeur();
  if(dom_interpol.nb_som()==0)
    {
      Cerr << "The domain \"" << dom_interpol.le_nom() << "\" to be split is empty !" << finl;
      exit();
    }
  const Zone& zone_interpol = dom_interpol.zone(0);
  const int nb_elem_interpol = zone_interpol.nb_elem_tot();
  const int dim_interpol = dom_interpol.dimension;

  // Domaine dom_calcul
  const Domaine dom_calcul = ref_domaine_calcul_.valeur();
  if(dom_calcul.nb_som()==0)
    {
      Cerr << "The reference domain \"" << dom_calcul.le_nom() << "\" is empty !" << finl;
      exit();
    }
  const Zone& zone_calcul = dom_calcul.zone(0);
  const int nb_elem_calcul = zone_calcul.nb_elem();
  const int dim_calcul = dom_calcul.dimension;

  IntVect elem_part_calcul;
  int nb_parts_tot_calcul;
  EFichier file;
  Nom nom_fichier;
  nom_fichier=nom_du_cas()+"_"+dom_calcul.le_nom()+".dec";
  file.ouvrir(nom_fichier);
  if (!file.good())
    {
      Cerr << "Error in Partitionneur_Partition::construire_partition\n";
      Cerr << " Failed to open file " << nom_fichier << finl;
      Cerr << "To use the partitioner Partition, you must check" << finl;
      Cerr << "that you use the option:" << finl;
      Cerr << "Ecrire_decoupage " << nom_fichier << finl;
      Cerr << "In the splitting block of the computational domain " << dom_calcul.le_nom() << finl;
      exit();
    }
  file >> elem_part_calcul;
  file >> nb_parts_tot_calcul;
  file.close();

  const int sz = elem_part_calcul.size_array();
  if (nb_elem_calcul != sz)
    {
      Cerr << "Error in Partitionneur_Partition::construire_partition\n";
      Cerr << " The file " << nom_fichier << " contains an array of " << sz << " values.\n";
      Cerr << " The area contains " << nb_elem_calcul << " elements" << finl;
      exit();
    }

  // Centre de gravite des elements de dom_interpol
  DoubleTab coord_g_interpol;
  //Calcul des centres de gravite des elements de dom_interpol
  zone_interpol.calculer_centres_gravite(coord_g_interpol);
  assert(coord_g_interpol.dimension(0) == nb_elem_interpol);
  assert(coord_g_interpol.dimension(1) == dim_interpol);

  // Centre de gravite des elements de dom_calcul
  DoubleTab coord_g_calcul;
  //Calcul des centres de gravite des elements de dom_calcul
  zone_calcul.calculer_centres_gravite(coord_g_calcul);
  assert(coord_g_calcul.dimension(0) == nb_elem_calcul);
  assert(coord_g_calcul.dimension(1) == dim_calcul);

  elem_part.resize(nb_elem_interpol);
  elem_part = 0;
  // Recherche des centres de gravite les plus proche les uns des autres
  {
    int i,j;
    double distance_min,distance;
    for (i = 0; i < nb_elem_interpol; i++)
      {
        distance_min=1e+9;
        double x_interpol = coord_g_interpol(i,0);
        double y_interpol = coord_g_interpol(i,1);
        double z_interpol = (dim_interpol == 3) ? coord_g_interpol(i,2) : 0.;
        for (j = 0; j < nb_elem_calcul; j++)
          {
            double x_calcul = coord_g_calcul(j,0);
            double y_calcul = coord_g_calcul(j,1);
            double z_calcul = (dim_calcul == 3) ? coord_g_calcul(j,2) : 0.;
            distance=sqrt((x_calcul-x_interpol)*(x_calcul-x_interpol)+(y_calcul-y_interpol)*(y_calcul-y_interpol)+(z_calcul-z_interpol)*(z_calcul-z_interpol));
            if (distance < distance_min)
              {
                distance_min=distance;
                elem_part[i]=elem_part_calcul[j];
              }
          }
      }
  }

  int nb_parties_interpol, nb_parties_calcul;
  nb_parties_interpol=0;
  nb_parties_calcul=0;
  // Nombre de proc sur elem_part
  if (elem_part.size_array() > 0)
    nb_parties_interpol = max_array(elem_part) + 1;
  // Nombre de proc sur elem_part_calcul
  if (elem_part_calcul.size_array() > 0)
    nb_parties_calcul = max_array(elem_part_calcul) + 1;
  // Si partie vide, on les prends en compte
  if (nb_parties_calcul < nb_parts_tot_calcul)
    nb_parties_calcul = nb_parts_tot_calcul;
  else
    nb_parts_tot_calcul = nb_parties_calcul;

  // Nombre de partie total a generer
  if ((nb_parties_interpol < nb_parties_calcul) && nb_parts_tot==-1)
    nb_parts_tot=nb_parties_calcul;
  if (nb_parts_tot_calcul < nb_parts_tot)
    nb_parts_tot=nb_parts_tot_calcul;
}
