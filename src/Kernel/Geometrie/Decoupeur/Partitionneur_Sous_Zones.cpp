/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Partitionneur_Sous_Zones.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie/Decoupeur
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////
#include <Partitionneur_Sous_Zones.h>

#include <Domaine.h>
#include <Sous_Zone.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Partitionneur_Sous_Zones,"Partitionneur_Sous_Zones",Partitionneur_base);

Partitionneur_Sous_Zones::Partitionneur_Sous_Zones()
{
}

// Description: Format de lecture:
//  (la liste de sous_zones est optionnelle, N est le nombre de sous_zones)
//  {
//     [ Sous_zones N nom_sous_zone1 nom_sous_zone2  nom_sous_zone3 ... ]
//  }
Entree& Partitionneur_Sous_Zones::readOn(Entree& is)
{
  if (! ref_domaine_.non_nul())
    {
      Cerr << " Error: the domain has not been associated" << finl;
      exit();
    }
  Partitionneur_base::readOn(is);
  Cerr << " Subareas names : ";
  return is;
}

Sortie& Partitionneur_Sous_Zones::printOn(Sortie& os) const
{
  Cerr << "Partitionneur_Sous_Zones::printOn invalid\n" << finl;
  exit();
  return os;
}

void Partitionneur_Sous_Zones::set_param(Param& param)
{
  param.ajouter("sous_zones",&noms_sous_zones_);
}

// Description:
//  Premiere etape d'initialisation du partitionneur: on associe un domaine.
void Partitionneur_Sous_Zones::associer_domaine(const Domaine& domaine)
{
  ref_domaine_ = domaine;
}

// Description:
//  Deuxieme etape d'initialisation: on definit les sous_zones a utiliser.
//  (on peut utiliser readOn a la place).
void Partitionneur_Sous_Zones::initialiser(const Noms& noms_sous_zones)
{
  assert(ref_domaine_.non_nul());
  noms_sous_zones_ = noms_sous_zones;
}

// Description:
//   Chaque sous-zone de noms_sous_zones_ definit les elements attribues
//   a un processeur.
//   Les processeurs sont attribues dans l'ordre d'apparition des
//   sous-zones dans le domaine (et non dans l'ordre d'apparition dans
//   la liste de sous-zones).
//   Premiere sous_zone qui figure dans la liste => proc 0
//   Element de la deuxieme sous_zone => proc 1
//   ...
//   Elements restants qui ne figurent dans aucune sous_zone => sur un nouveau pe.
//   Si un element figure dans plusieurs sous-zones, c'est la premiere sous_zone
//   qui gagne.
void Partitionneur_Sous_Zones::construire_partition(ArrOfInt& elem_part, int& nb_parts_tot) const
{
  assert(ref_domaine_.non_nul());
  const Domaine& dom = ref_domaine_.valeur();
  const Zone& zone = dom.zone(0);
  const int nb_elem = zone.nb_elem();
  elem_part.resize_array(nb_elem);
  elem_part = -1;
  const int nb_sous_zones = dom.nb_ss_zones();

  const int toutes_sous_zones = (noms_sous_zones_.size() == 0);
  if (toutes_sous_zones)
    Cerr << " No subarea specified, we use all existing subareas." << finl;

  // Numero de processeur en cours d'attribution:
  int pe = 0;
  int count = 0;
  for (int i_sous_zone = 0; i_sous_zone < nb_sous_zones; i_sous_zone++)
    {
      const Sous_Zone& sous_zone = dom.ss_zone(i_sous_zone);
      const Nom& nom = sous_zone.le_nom();

      if (! toutes_sous_zones && ! noms_sous_zones_.contient_(nom))
        continue;

      Cerr << " Allocation of elements of the subarea " << nom << " to the processor " << pe << finl;
      count = 0;
      const int nb_elem_ssz = sous_zone.nb_elem_tot();
      for (int i = 0; i < nb_elem_ssz; i++)
        {
          const int elem = sous_zone[i];
          if (elem_part[elem] < 0)
            {
              count++;
              elem_part[elem] = pe;
            }
        }
      Cerr << " Number of elements attributed to the processor " << pe << " : " << count << finl;
      pe++;
    }
  Cerr << " Allocation of the remaining elements to the processor " << pe << " : ";
  count = 0;
  for (int elem = 0; elem < nb_elem; elem++)
    {
      if (elem_part[elem] < 0)
        {
          count++;
          elem_part[elem] = pe;
        }
    }
  Cerr << count << " elements." << finl;

  if (liste_bords_periodiques_.size() > 0)
    corriger_bords_avec_liste(dom, liste_bords_periodiques_, elem_part);

  // On ne corrige pas elem0 sur proc0 (cas test CouplageFluide_Pb1Pb2_VEF_CN)
}
