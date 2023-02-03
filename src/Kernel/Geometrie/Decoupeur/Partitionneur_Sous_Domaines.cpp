/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Partitionneur_Sous_Domaines.h>
#include <Synonyme_info.h>
#include <Domaine.h>
#include <Sous_Domaine.h>
#include <Param.h>
#include <Interprete.h>

Implemente_instanciable_sans_constructeur(Partitionneur_Sous_Domaines,"Partitionneur_Sous_Domaines",Partitionneur_base);
// XD partitionneur_sous_domaines partitionneur_deriv partitionneur_sous_domaines -1 This algorithm will create one part for each specified subdomaine/domain. All elements contained in the first subdomaine/domain are put in the first part, all remaining elements contained in the second subdomaine/domain in the second part, etc... NL2 If all elements of the current domain are contained in the specified subdomaines/domain, then N parts are created, otherwise, a supplemental part is created with the remaining elements. NL2 If no subdomaine is specified, all subdomaines defined in the domain are used to split the mesh.
Add_synonym(Partitionneur_Sous_Domaines, "Partitionneur_Sous_Zones");

Partitionneur_Sous_Domaines::Partitionneur_Sous_Domaines()
{
}

Entree& Partitionneur_Sous_Domaines::readOn(Entree& is)
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

Sortie& Partitionneur_Sous_Domaines::printOn(Sortie& os) const
{
  Cerr << "Partitionneur_Sous_Domaines::printOn invalid\n" << finl;
  exit();
  return os;
}

/*! @brief Format de lecture: (la liste de sous_domaines/domaines est optionnelle, N est le nombre d'entites lues)
 *
 *   {
 *      [ Sous_domaines N nom_sous_domaine1 nom_sous_domaine2  nom_sous_domaine3 ... ]
 *      [ Domaines N nom_domaine1 nom_domaine2 ... ]
 *   }
 *
 */
void Partitionneur_Sous_Domaines::set_param(Param& param)
{
  param.ajouter("sous_zones",&noms_sous_domaines_);  // XD attr sous_zones listchaine sous_zones 1 N SUBZONE_NAME_1 SUBZONE_NAME_2 ...
  param.ajouter("domaines",&noms_domaines_);         // XD attr domaines   listchaine domaines   1 N DOMAIN_NAME_1  DOMAIN_NAME_2  ...
}

/*! @brief Premiere etape d'initialisation du partitionneur: on associe un domaine.
 *
 */
void Partitionneur_Sous_Domaines::associer_domaine(const Domaine& domaine)
{
  ref_domaine_ = domaine;
}

/*! @brief Deuxieme etape d'initialisation: on definit les sous_domaines a utiliser.
 *
 * (on peut utiliser readOn a la place).
 *
 */
void Partitionneur_Sous_Domaines::initialiser(const Noms& noms_sous_domaines)
{
  assert(ref_domaine_.non_nul());
  noms_sous_domaines_ = noms_sous_domaines;
}

/*! @brief Chaque sous-domaine de noms_sous_domaines_ definit les elements attribues a un processeur.
 *
 *    Les processeurs sont attribues dans l'ordre d'apparition des
 *    sous-domaines/domaines dans le domaine (et non dans l'ordre d'apparition dans
 *    la liste de sous-domaines).
 *    Premiere sous_domaine/domaine qui figure dans la liste => proc 0
 *    Element de la deuxieme sous_domaine => proc 1
 *    ...
 *    Elements restants qui ne figurent dans aucune sous_domaine => sur un nouveau pe.
 *    Si un element figure dans plusieurs sous-domaines, c'est la premiere sous_domaine
 *    qui gagne.
 *
 */
void Partitionneur_Sous_Domaines::construire_partition(IntVect& elem_part, int& nb_parts_tot) const
{
  assert(ref_domaine_.non_nul());
  const Domaine& dom = ref_domaine_.valeur();
  const int nb_elem = dom.nb_elem_tot();
  elem_part.resize(nb_elem);
  elem_part = -1;
  if (noms_domaines_.size()!=0 && noms_sous_domaines_.size())
    {
      Cerr << "Can't mix sous_domaines and domaines yet in sous_domaines partitionner." << finl;
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
              // Contrairement au sous domaine, le domaine doit exister pour le moment
              Cerr << "Domain " << nom_domaine << " is not existing." << finl;
              Process::exit();
              // ToDo eventuellement liste de domaines dans des fichiers ex: DOM_0000.Zones, DOM_0001.Zones
            }
          const Domaine& domaine = ref_cast(Domaine, interprete().objet(nom_domaine));
          DoubleTab domaine_xp;
          domaine.calculer_centres_gravite(domaine_xp);
          IntVect dom_cells_containing_domaine_cells;
          dom.chercher_elements(domaine_xp, dom_cells_containing_domaine_cells);
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
      const int nb_sous_domaines = dom.nb_ss_domaines();
      const int toutes_sous_domaines = (noms_sous_domaines_.size() == 0);
      if (toutes_sous_domaines)
        Cerr << " No subarea specified, we use all existing subareas." << finl;
      for (int i_sous_domaine = 0; i_sous_domaine < nb_sous_domaines; i_sous_domaine++)
        {
          const Sous_Domaine& sous_domaine = dom.ss_domaine(i_sous_domaine);
          const Nom& nom = sous_domaine.le_nom();
          bool sous_domaine_trouvee = noms_sous_domaines_.contient_(nom);
          if (!toutes_sous_domaines && !sous_domaine_trouvee)
            continue;

          Cerr << " Allocation of elements of the subarea " << nom << " to the processor " << pe << finl;
          count = 0;
          const int nb_elem_ssz = sous_domaine.nb_elem_tot();
          for (int i = 0; i < nb_elem_ssz; i++)
            {
              const int elem = sous_domaine[i];
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
      Cerr << "Error! The last domaine will be empty with 0 elements. It is surely not what you want." << finl;
      Cerr << "Check, your partition strategy." << finl;
      Process::exit();
    } */
  if (liste_bords_periodiques_.size() > 0)
    corriger_bords_avec_liste(dom, liste_bords_periodiques_, 0, elem_part);

  // On ne corrige pas elem0 sur proc0 (cas test CouplageFluide_Pb1Pb2_VEF_CN)
}
