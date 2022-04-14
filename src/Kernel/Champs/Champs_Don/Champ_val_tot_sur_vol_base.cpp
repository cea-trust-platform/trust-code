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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Champ_val_tot_sur_vol_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs/Champs_Don
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_val_tot_sur_vol_base.h>
#include <Interprete.h>
#include <Domaine.h>
#include <Zone_VF.h>
#include <Sous_Zone.h>

Implemente_base(Champ_val_tot_sur_vol_base,"Champ_val_tot_sur_vol_base",Champ_Uniforme_Morceaux);


Sortie& Champ_val_tot_sur_vol_base::printOn(Sortie& os) const
{
  return Champ_Uniforme_Morceaux::printOn(os);
}

Entree& Champ_val_tot_sur_vol_base::readOn(Entree& is)
{
  Champ_Uniforme_Morceaux::readOn(is);
  return is;
}

//Evaluation du champ sur le domaine par defaut et les sous zones specifiees
//-Pour chacune des sous zones et pour le domaine complet on calcul CONTRIB_loc:
//   loc designe une localisation (sous zone i ou domaine par defaut)
//   cas VDF : CONTRIB_loc = somme de volume(elem)*porosite(elem) pour les elements de loc
//   cas VEF : CONTRIB_loc = somme de vol_entrelaces(face)*por_face(face) pour les faces de loc
//-On retranche CONTRIB_loc de chaque sous zone a celle du domaine complet
//-Estimation de la puissance aux elements pour chaque localisation par la relation :
//  P(elem,compo) = P_lue/CONTRIB_loc
//
void Champ_val_tot_sur_vol_base::evaluer(const Zone_dis_base& zdis,const Zone_Cl_dis_base& zcldis)
{
  const int nb_elem = zdis.nb_elem();
  DoubleVect vol_glob_pond;
  vol_glob_pond = eval_contrib_loc(zdis,zcldis,vol_glob_pond);

  for(int elem=0; elem<nb_elem; elem++)
    for (int k=0; k<nb_compo_; k++)
      valeurs_(elem,k) /= vol_glob_pond(0);

  int cpt=1;
  LIST_CURSEUR(REF(Sous_Zone)) curseur1 = les_sous_zones;
  while(curseur1)
    {
      const Sous_Zone& sz = curseur1.valeur().valeur();
      int size_sz = sz.nb_elem_tot();
      int el;
      for (int elem=0; elem<size_sz; elem++)
        {
          el = sz(elem);
          for (int k=0; k<nb_compo_; k++)
            {
              valeurs_(el,k) *= vol_glob_pond(0);
              valeurs_(el,k) /= vol_glob_pond(cpt);
            }
        }

      cpt++;
      ++curseur1;
    }

  valeurs().echange_espace_virtuel();
}

