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
// File:        Domaine_dis.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/27
//
//////////////////////////////////////////////////////////////////////////////

#include <Domaine_dis.h>
#include <Domaine.h>
#include <Discretisation_base.h>
#include <Zone_VF.h>

Implemente_instanciable(Domaine_dis,"Domaine_dis",Objet_U);


// Description:
//    Surcharge Objet_U::printOn(Sortie&)
//    Imprime le domaine discretise sur un flot de sortie.
//    Imprime le nom du domaine et les zones qu'il contient.
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Domaine_dis::printOn(Sortie& os) const
{
  return os << domaine().le_nom() << finl << les_zones << les_sous_zones_dis;
}


// Description:
//    Lit le domaine discretise a partir d'un flot d'entree.
//    Le format attendu est le suivant:
//       nom_domaine
//       bloc de lecture Zones_Dis
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Domaine_dis::readOn(Entree& is)
{
  Nom nom;
  return is >> nom >> les_zones >> les_sous_zones_dis;
}


// Description:
//    Discretise le domaine.
//    On type et on discretise chaque zone discretisee du domaine discretise
//    grace a la discretisation passee en parametre.
// Precondition: le domaine associe doit etre non nul
// Parametre: Discretisation_base& discretisation
//    Signification: la discretisation a appliquer a chaque zone du domaine
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: les zones du domaines discretise sont discretisees.
void Domaine_dis::discretiser(const Nom& type_1)
{
  Nom type(type_1);
  int face_ok=1;
  type.suffix("NO_FACE_");
  if (type!=type_1) face_ok=0;
  if (!le_domaine.non_nul())
    {
      Cerr << "The domain is not specified. Please check your data file." << finl;
      exit();
    }
  const Domaine& dom=le_domaine.valeur();

  // Initialisation du tableau renum_som_perio
  const int nb_som = dom.nb_som();
  IntTab renum(nb_som);
  for (int i = 0; i < nb_som; i++)
    renum[i] = i;
  le_domaine.valeur().set_renum_som_perio(renum);

  // Cree les Zone_dis, les type et leur associe la zone et le domaine
  // correspondants.

  les_zones.dimensionner(dom.nb_zones());

  for(int i=0; i<nombre_de_zones(); i++)
    {
      zone_dis(i).typer(type);
      zone_dis(i).associer_zone(dom.zone(i));
      if (face_ok)
        zone_dis(i).discretiser();
      else
        {
          Zone& zone_geom = domaine().zone(0);
          Zone_VF& zvf = ref_cast(Zone_VF,zone_dis(i).valeur());
          zvf.typer_elem(zone_geom);
          // Calcul du volume de la zone discretisee
          zone_geom.calculer_volumes(zvf.volumes(),zvf.inverse_volumes());
        }
      zone_dis(i).associer_domaine_dis(*this);
    }

  // Cree les sous_zones_dis, les type, et leur associe
  // les zone_dis et les sous_zone correspondantes.

  les_sous_zones_dis.dimensionner(dom.nb_ss_zones());

  for (int i=0; i<dom.nb_ss_zones(); i++)
    {

      // Cherche la Zone_dis associee
      const Zone& la_zone=dom.ss_zone(i).zone();
      int z=0;
      while(z<nombre_de_zones() && !zone_dis(z).zone().est_egal_a(la_zone))
        z++;
      if (z==nombre_de_zones())
        {
          Cerr << "In Domaine_dis::discretiser, impossible to find a "
               << "zone_dis corresponding to sous_zone " << i << finl;
          exit();
        }
      const Zone_dis& la_zone_dis=zone_dis(z);
      if (face_ok)
        {
          if (sub_type(Zone_VF,la_zone_dis.valeur()))
            {
              sous_zone_dis(i).typer("Sous_zone_VF");
              sous_zone_dis(i).associer_sous_zone(dom.ss_zone(i));
              sous_zone_dis(i).associer_zone_dis(la_zone_dis.valeur());
              sous_zone_dis(i).discretiser();
            }
          else
            {
              Cerr << "Domaine_dis::discretiser : there is no sub-areas discretized "
                   << "for discretized areas which not deriving from Zone_VF : "
                   << la_zone_dis.valeur().que_suis_je() << finl;
              exit();
            }
        }
    }
}


// Description:
//    Associe un Domaine (non discretise) a l'objet.
//    Dimensionne le tableau des zones discretisees associees
//    au domaine.
// Precondition:
// Parametre: Domaine& un_domaine
//    Signification: le domaine a associer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Domaine_dis::associer_domaine(const Domaine& un_domaine)
{
  le_domaine=un_domaine;
}
