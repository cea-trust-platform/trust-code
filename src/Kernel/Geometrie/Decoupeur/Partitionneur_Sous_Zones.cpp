/****************************************************************************
* Copyright (c) 2021, CEA
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
// XD partitionneur_sous_zones partitionneur_deriv sous_zones -1 This algorithm will create one part for each specified subzone/domain. All elements contained in the first subzone/domain are put in the first part, all remaining elements contained in the second subzone/domain in the second part, etc... NL2 If all elements of the current domain are contained in the specified subzones/domain, then N parts are created, otherwise, a supplemental part is created with the remaining elements. NL2 If no subzone is specified, all subzones defined in the domain are used to split the mesh.

Partitionneur_Sous_Zones::Partitionneur_Sous_Zones()
{
}

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

// Description: Format de lecture:
//  (la liste de sous_zones/domaines est optionnelle, N est le nombre d'entites lues)
//  {
//     [ Sous_zones N nom_sous_zone1 nom_sous_zone2  nom_sous_zone3 ... ]
//     [ Domaines N nom_domaine1 nom_domaine2 ... ]
//  }
void Partitionneur_Sous_Zones::set_param(Param& param)
{
  param.ajouter("sous_zones",&noms_sous_zones_);     // XD attr sous_zones listchaine sous_zones 1 N SUBZONE_NAME_1 SUBZONE_NAME_2 ...
  param.ajouter("domaines",&noms_domaines_);         // XD attr domaines   listchaine domaines   1 N DOMAIN_NAME_1  DOMAIN_NAME_2  ...
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
//   sous-zones/domaines dans le domaine (et non dans l'ordre d'apparition dans
//   la liste de sous-zones).
//   Premiere sous_zone/domaine qui figure dans la liste => proc 0
//   Element de la deuxieme sous_zone => proc 1
//   ...
//   Elements restants qui ne figurent dans aucune sous_zone => sur un nouveau pe.
//   Si un element figure dans plusieurs sous-zones, c'est la premiere sous_zone
//   qui gagne.
void Partitionneur_Sous_Zones::construire_partition(IntTab& elem_part, int& nb_parts_tot) const
{
  assert(ref_domaine_.non_nul());
  const Domaine& dom = ref_domaine_.valeur();
  const Zone& zone = dom.zone(0);
  const int nb_elem = zone.nb_elem_tot();
  elem_part.resize(nb_elem);
  elem_part = -1;
  if (noms_domaines_.size()!=0 && noms_sous_zones_.size())
    {
      Cerr << "Can't mix sous_zones and domaines yet in sous_zones partitionner." << finl;
      Process::exit();
    }
  // Numero de processeur en cours d'attribution:
  int pe = 0;
  int count = 0;
  if (noms_domaines_.size()!=0)
    {
      for (int i=0; i<noms_domaines_.size(); i++)
        {
          const Nom& nom_domaine = noms_domaines_[i];
          if (!interprete().objet_existant(nom_domaine) || !sub_type(Domaine, interprete().objet(nom_domaine)))
            {
              // Contrairement au sous zone, le domaine doit exister pour le moment
              Cerr << "Domain " << nom_domaine << " is not existing." << finl;
              Process::exit();
              // ToDo eventuellement liste de zones dans des fichiers ex: DOM_0000.Zones, DOM_0001.Zones
            }
          const Domaine& domaine = ref_cast(Domaine, interprete().objet(nom_domaine));
          DoubleTab domaine_xp;
          domaine.zone(0).calculer_centres_gravite(domaine_xp);
          IntVect dom_cells_containing_domaine_cells;
          dom.zone(0).chercher_elements(domaine_xp, dom_cells_containing_domaine_cells);
          Cerr << " Allocation of elements of the domain " << domaine.le_nom() << " to the processor " << pe << finl;
          for (int cell = 0; cell < dom_cells_containing_domaine_cells.size(); cell++)
            {
              int elem = dom_cells_containing_domaine_cells[cell];
              if (elem_part[elem] < 0)
                {
                  count++;
                  elem_part[elem] = pe;
                }
            }
          Cerr << " Number of elements attributed to the processor " << pe << " : " << count << finl;
          pe++;
        }
    }
  else
    {
      const int nb_sous_zones = dom.nb_ss_zones();
      const int toutes_sous_zones = (noms_sous_zones_.size() == 0);
      if (toutes_sous_zones)
        Cerr << " No subarea specified, we use all existing subareas." << finl;
      for (int i_sous_zone = 0; i_sous_zone < nb_sous_zones; i_sous_zone++)
        {
          const Sous_Zone& sous_zone = dom.ss_zone(i_sous_zone);
          const Nom& nom = sous_zone.le_nom();
          bool sous_zone_trouvee = noms_sous_zones_.contient_(nom);
          if (!toutes_sous_zones && !sous_zone_trouvee)
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
  /*
  if (count==0)
    {
      Cerr << "Error! The last zone will be empty with 0 elements. It is surely not what you want." << finl;
      Cerr << "Check, your partition strategy." << finl;
      Process::exit();
    } */
  if (liste_bords_periodiques_.size() > 0)
    corriger_bords_avec_liste(dom, liste_bords_periodiques_, 0, elem_part);

  // On ne corrige pas elem0 sur proc0 (cas test CouplageFluide_Pb1Pb2_VEF_CN)
}
